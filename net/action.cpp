#include "action.h"
#include "const.h"


Action::Action(ActionCB callback)
{
    this->callback = callback;
    id = 0;
    send_time = std::time(nullptr);
    status = AS_PENDING;
    error_message = "";
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


json Action::create_request()
{
    return {
        { "cid", get_command_id() },
        { "mid", id },
        { "cdata", create_request_impl() }
    };
}


void Action::handle_response(int status, json response, string error)
{
    if(status == SERV_RESP_SUCCESS)
        this->status = handle_success_response_impl(response);

    else if(status == SERV_RESP_FAILURE)
        this->status = AS_FAIL;

    error_message = error;
    callback(this);
}


float Action::get_timeout_seconds() { return 2.0; }


void Action::do_timeout()
{
    status = AS_TIMEOUT;
    callback(this);
}

