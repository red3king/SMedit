#include "arg.h"


Arg::Arg(string variable_name, LVOV argument_value)
{
    this->variable_name = variable_name;
    this->argument_value = argument_value;
}


string Arg::describe()
{
    return variable_name + ": " + argument_value.to_string();
}
