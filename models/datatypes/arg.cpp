#include "arg.h"


Arg::Arg(string variable_name, LVOV argument_value)
{
    this->variable_name = variable_name;
    this->argument_value = argument_value;
}


Arg::Arg(json jdata) : argument_value(jdata["argument_value"])
{
    variable_name = jdata["variable_name"];
}


string Arg::describe()
{
    return variable_name + ": " + argument_value.to_string();
}


json Arg::to_json()
{
    return {
        { "variable_name", variable_name },
        { "argument_value", argument_value.to_json() }   
    };
}
