#pragma once

#include <string>
#include "lib/json.hpp"

#include "const.h"

using std::string;
using nlohmann::json;


class ArgDef
{
    public:
        ArgDef(string variable_name, ValueType value_type);
        ArgDef(json jdata);

        string variable_name;        
        ValueType value_type;

        json to_json();

        string describe();
};
