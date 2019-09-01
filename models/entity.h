#pragma once

#include <array>
#include "lib/json.hpp"

using nlohmann::json;


enum EntityType
{
    NONE_ENTITY,
    MACHINE,
    RESOURCE,
    STATE,
    TRANSITION,
    RESOURCELOCK
};


class Entity 
{
    public:
        Entity(unsigned int id=0);
        Entity(json jdata);
        Entity(const Entity& other);

        virtual ~Entity() = 0;        
        json to_json();

        unsigned int id;  // ids start at 1, 0 is reserved for representing null to functions        

    private:

};


extern std::array<EntityType, 3> gui_update_order;
