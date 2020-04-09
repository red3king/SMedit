#pragma once

#include <string>
#include "lib/json.hpp"

#include "entity.h"

using nlohmann::json;
using std::string;


class Resource : public Entity
{
    public:
        Resource(unsigned int id=0);
        Resource(json jdata);

        json to_json();
        
        EntityType get_entity_type();

        string path;
        string name;
};
