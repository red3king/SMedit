#pragma once

#include <array>


enum EntityType
{
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

        unsigned int id;        

    private:

};


extern std::array<EntityType, 3> gui_update_order;
