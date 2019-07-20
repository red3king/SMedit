#include "entity.h"


Entity::Entity(unsigned int id)
{
    this->id = id;
}

Entity::~Entity() { }

std::array<EntityType, 3> gui_update_order = { TRANSITION, STATE, RESOURCELOCK };
