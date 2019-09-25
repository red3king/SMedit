#pragma once

#include "net/action.h"


class Echo : public Action
{
    public:
        Echo(ActionCB callback);
        string get_command_id();
        json create_request_impl();
        ActionStatus handle_success_response_impl(json response);
};


