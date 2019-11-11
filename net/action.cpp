#include "action.h"
#include "const.h"


Action::Action(ActionCB callback)
{
    this->callback = callback;
    _init();
}


Action::Action()
{
    _init();
}


void Action::_init()
{
    id = 0;
    send_time = std::time(nullptr);
    status = AS_PENDING;
    error_message = "";
    next_action = nullptr;
}


Action::~Action() { }


void Action::assign_id(int id)
{
    this->id = id;
}


bool Action::has_timed_out()
{
    std::time_t current = std::time(nullptr);
    return std::difftime(current, send_time) > get_timeout_seconds();
}


void Action::add_next_action(Action* next_action)
{
    if(this->next_action == nullptr)
        this->next_action = next_action;

    else
        this->next_action->add_next_action(next_action);
}


bool Action::has_next_action()
{
    return next_action != nullptr;
}


Action* Action::get_next_action()
{
    return next_action;
}


void Action::fail_chain(string failed_action_error)
{
    status = AS_FAIL;
    error_message = failed_action_error;
    
    if(callback)
        callback(this);
 
    if(next_action == nullptr)
        return;
       
    next_action->fail_chain(failed_action_error);
    delete next_action;
}


json Action::create_request()
{
    return {
        { "cid", get_command_id() },
        { "mid", id },
        { "cdata", create_request_impl() }
    };
}


json Action::create_request_impl()
{
    return {};
}


ActionStatus Action::handle_success_response_impl(json response) { return AS_SUCCESS; }


ActionStatus Action::handle_response(int status, json response, string error)
{
    if(status == SERV_RESP_SUCCESS)
        this->status = handle_success_response_impl(response);

    else if(status == SERV_RESP_FAILURE)
        this->status = AS_FAIL;

    error_message = error;
    
    if(callback)
        callback(this);
    
    return this->status;
}


float Action::get_timeout_seconds() { return 2.0; }


void Action::do_timeout()
{
    status = AS_TIMEOUT;

    if(callback)
        callback(this);

    if(next_action != nullptr)
    {
        next_action->do_timeout();
        delete next_action;
    }
}

