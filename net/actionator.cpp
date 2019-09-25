#include "lib/base64/base64.h"

#include "actionator.h"
#include "const.h"
#include "log.h"


Actionator::Actionator(LineClient* line_client)
{
    action_id_ctr = 0;
    this->line_client = line_client;
    line_client->line_received_signal.connect(sigc::mem_fun(this, &Actionator::on_line_received));
}


void Actionator::submit_action(Action* action)
{
    action->assign_id(action_id_ctr++);
    json request = action->create_request();
    string jstr = request.dump();
    string result = base64_encode((unsigned char*)jstr.c_str(), jstr.length()) + "\r\n";
    current_actions.push_back(action);
    line_client->send(result);
}


void Actionator::on_line_received(string line)
{
    json response;

    try { line = base64_decode(line); }
    catch (...) { log("actionator: not base64!"); return; }

    log("Actionator::on_line_received(" + line + ")");

    try { response = json::parse(line); }
    catch (...) { log("actionator: line invalid"); return; }

    int message_id;
    int status;
    string error;
    json result;

    try 
    {
        message_id = response["mid"];
        status = response["status"];
        log("status=" + std::to_string(status));
    }

    catch (nlohmann::detail::type_error& ex)
    {
        log("actionator: missing field");
        return;
    }

    if(status == SERV_RESP_SUCCESS)
    {
        try { result = response["result"]; }
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
    for(i=0; i<current_actions.size(); i++)
    {
        if(current_actions[i]->id == message_id)
        {
            found = true;
            current_actions[i]->handle_response(status, response, error);
            break;
        }
    }

    if(!found)
        log("Actionator: couldnt find action matching response!!!!");

    // Remove finished action
    if(i < current_actions.size())
    {
        delete current_actions[i];
        current_actions.erase(current_actions.begin() + i);
    }

    _purge_timed_out();
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
