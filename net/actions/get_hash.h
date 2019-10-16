#pragma once

#include "net/action.h"



class GetHash : public Action 
{
    public:
        uint16_t project_hash;

        GetHash(ActionCB callback);
        GetHash();
        string get_command_id();
        ActionStatus handle_success_response_impl(json response);
};

