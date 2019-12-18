#pragma once

#include "net/action.h"


class StateSynchAction : public Action
{
    public:
        StateSynchAction(ActionCB callback);

        string get_command_id();
        ActionStatus handle_success_response_impl(json response);

        json current_running_machines;
        bool project_started;
        bool project_paused;

    private:
};
