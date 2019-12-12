#include "lib/base64/base64.h"

#include "actionator.h"
#include "const.h"
#include "log.h"


Actionator::Actionator(LineClient* line_client, BroadcastEvents* broadcast_events)
{
    action_id_ctr = 0;
    this->line_client = line_client;
    this->broadcast_events = broadcast_events;
    line_client->line_received_signal.connect(sigc::mem_fun(this, &Actionator::on_line_received));
    line_client->disconnection_signal.connect(sigc::mem_fun(this, &Actionator::on_disconnect));
}


void Actionator::submit_action(Action* action)
{
    action->assign_id(action_id_ctr++);
    json request = action->create_request();
    string jstr = request.dump();
    log("actionator: >>> SEND >>> " + jstr);
    string result = base64_encode((unsigned char*)jstr.c_str(), jstr.length()) + "\r\n";
    current_actions.push_back(action);
    line_client->send(result);
}


void Actionator::on_disconnect(bool intentional)
{
    cancel_all();
}


void Actionator::cancel_action(Action* action)
{
    int i=0;
    for(; i<current_actions.size(); i++)
    {
        auto cur = current_actions[i];
        if(action == cur)
        {
            action->do_timeout();
            delete action;
            break;
        }
    }

    if(i < current_actions.size())
        current_actions.erase(current_actions.begin() + i);
}


void Actionator::cancel_all()
{
    for(int i=0; i<current_actions.size(); i++)
    {
        auto cur = current_actions[i];
        cur->do_timeout();
        delete cur;
    }

    while(current_actions.size())
        current_actions.erase(current_actions.begin());
}


void Actionator::on_line_received(string line)
{
    json response;

    try { line = base64_decode(line); }
    catch (...) { log("actionator: not base64!"); return; }

    log("actionator: <<< RECV <<< " + line);

    try { response = json::parse(line); }
    catch (...) { log("actionator: line invalid"); return; }

    bool is_broadcast;

    try { is_broadcast = response["broadcast"]; }

    catch(nlohmann::detail::type_error& ex)
    {
        log("actionator: missing broadcast field");
        return;
    }

    if(is_broadcast)
        handle_broadcast(response);

    else
        handle_response(response);

}


void Actionator::handle_broadcast(json broadcast)
{
    broadcast_events->handle_broadcast(broadcast);
}


void Actionator::handle_response(json response)
{
    int message_id;
    int status;
    string error;
    json result;

    try 
    {
        message_id = response["mid"];
        status = response["status"];
    }

    catch (nlohmann::detail::type_error& ex)
    {
        log("actionator: missing field");
        return;
    }

    if(status == SERV_RESP_SUCCESS)
    {
        try { result = response["resp"]; }
        catch (...) { log("actionator: missing result"); return; }
    }

    else if(status == SERV_RESP_FAILURE)
    {
        try { error = response["err"]; }
        catch (...) { log("actionator: missing err"); return; }
    }
    
    // Find action to handle response
    int i;
    bool found = false;
    ActionStatus action_status;
    for(i=0; i<current_actions.size(); i++)
    {
        if(current_actions[i]->id == message_id)
        {
            found = true;
            action_status = current_actions[i]->handle_response(status, result, error);
            break;
        }
    }

    if(!found)
        log("Actionator: couldnt find action matching response!!!!");

    Action* next_action = nullptr;

    // Remove finished action
    if(i < current_actions.size())
    {
        auto action = current_actions[i];
        next_action = action->get_next_action();
        delete action;
        current_actions.erase(current_actions.begin() + i);
    }

    _purge_timed_out();

    // Handle chained actions
    if(next_action != nullptr)
    {
        if(action_status == AS_SUCCESS)
            submit_action(next_action);
        else
            next_action->fail_chain("prev action in chain failed:\r\n" + error);  // fails callback, and deletes this and subsequent actions
    }
}


void Actionator::_purge_timed_out()
{
    for(int i=0; i<current_actions.size(); i++)
    {
        if(current_actions[i]->has_timed_out())
        {
            current_actions[i]->do_timeout();
            delete current_actions[i];
            current_actions.erase(current_actions.begin() + i--);
        }
    }
}
