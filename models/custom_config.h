#pragma once

#include "const.h"
#include "models/datatypes/lvov.h"


class CustomConfigDef
{
    public:
        CustomConfigDef(json jdata);
        
        string name;
        ValueType type;
        
        json to_json();
};



class CustomStateConfig
{
    public:
        CustomStateConfig();
        CustomStateConfig(string name, LVOV value);     // create with value
        CustomStateConfig(string name, ValueType value_type);        // create with default value of type
        CustomStateConfig(json jdata);                  
        
        LVOV value;
        string name;
        
        ValueType get_value_type();
        json to_json();
};
        
        
