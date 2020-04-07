#include "lib/json.hpp"

#include "custom_config.h"



///////  CustomConfigDef   /////////////

CustomConfigDef::CustomConfigDef(json jdata)
{
    name = jdata["name"];
    type = (ValueType) jdata["type"];
}


json CustomConfigDef::to_json()
{
    return {
        { "name", name },
        { "type", (int) type }
    };
}



///// CustomStateConfig

CustomStateConfig::CustomStateConfig() { }


CustomStateConfig::CustomStateConfig(string name, LVOV value)
{
    this->name = name;
    this->value = value;
}


CustomStateConfig::CustomStateConfig(json jdata)
{
    name = jdata["name"];
    value = LVOV(jdata["lvov"]);
}


CustomStateConfig::CustomStateConfig(string name, ValueType value_type)
{
    this->name = name;
    value = LVOV(value_type);
}


json CustomStateConfig::to_json()
{
    return 
    {
        { "name", name },
        { "lvov", value.to_json() }        
    };
}
