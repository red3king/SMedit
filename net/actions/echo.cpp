#include "echo.h"


Echo::Echo(ActionCB callback) : Action(callback) { }


string Echo::get_command_id() { return "echo"; }


json Echo::create_request_impl() { return {}; }


ActionStatus Echo::handle_success_response_impl(json response) { return AS_SUCCESS; }





