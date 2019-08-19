#include "argdef.h"


ArgDef::ArgDef(string variable_name, ValueType value_type)
{
    this->variable_name = variable_name;
    this->value_type = value_type;
}


string ArgDef::describe()
{
    return variable_name + " - " + value_type_to_string(value_type);
}


