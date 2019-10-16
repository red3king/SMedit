#include "get_hash.h"


GetHash::GetHash(ActionCB callback) : Action(callback)
{
    project_hash = 0;
}


GetHash::GetHash() : Action()
{
    project_hash = 0;
}


string GetHash::get_command_id() {  return "project hash"; }


ActionStatus GetHash::handle_success_response_impl(json response)
{
    project_hash = response["hash"];
    return AS_SUCCESS;
}
