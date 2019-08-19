#pragma once

#include "lvov.h"


class Arg
{
    public:
        Arg(string variable_name, LVOV argument_value);
        
        string variable_name;
        LVOV argument_value;
};
