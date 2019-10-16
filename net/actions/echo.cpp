#include "echo.h"


Echo::Echo(ActionCB callback) : Action(callback) { }


string Echo::get_command_id() { return "echo"; }




