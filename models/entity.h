#pragma once

#include <array>


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
        virtual ~Entity() = 0;        

        unsigned int id;  // ids start at 1, 0 is reserved for representing null to functions        

    private:

};


extern std::array<EntityType, 3> gui_update_order;
