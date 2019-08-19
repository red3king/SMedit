#pragma once

#include <string>

#include "const.h"

using std::string;


class ArgDef
{
    public:
        ArgDef(string variable_name, ValueType value_type);

        string variable_name;        
        ValueType value_type;

        string describe();
};
