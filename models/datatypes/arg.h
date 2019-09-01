#pragma once

#include "lib/json.hpp"

#include "lvov.h"

using nlohmann::json;


class Arg
{
    public:
        Arg(string variable_name, LVOV argument_value);
        Arg(json jdata);

        string variable_name;
        LVOV argument_value;

        json to_json();

        string describe();
};
