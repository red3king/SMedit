#include "resource.h"


Resource::Resource(unsigned int id) : Entity(id) { } 


Resource::Resource(json jdata) : Entity(jdata)
{
    name = jdata["name"];
    path = jdata["path"];
}


json Resource::to_json()
{
    json jdata = Entity::to_json();
    jdata["name"] = name;
    jdata["path"] = path;
    return jdata;
}
