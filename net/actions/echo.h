#pragma once

#include "net/action.h"


class Echo : public Action
{
    public:
        Echo(ActionCB callback);
        string get_command_id();
};


