#include "state_synch.h"


StateSynchAction::StateSynchAction(ActionCB callback) : Action(callback)
{
    project_started = false;
    project_paused = false;
}


string StateSynchAction::get_command_id()
{
    return "state synch";
}


ActionStatus StateSynchAction::handle_success_response_impl(json response)
{
    project_started = response["project_started"];
    project_paused = response["project_paused"];
    current_running_machines = response["machines_list"];
    return AS_SUCCESS;
}
