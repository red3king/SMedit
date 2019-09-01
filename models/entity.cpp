#include "entity.h"


Entity::Entity(unsigned int id)
{
    this->id = id;
}


Entity::Entity(json jdata)
{
    id = jdata["id"];
}


Entity::Entity(const Entity& other)
{
    id = other.id;
}


Entity::~Entity() { }


json Entity::to_json()
{
    return { { "id", id } };
}


std::array<EntityType, 3> gui_update_order = { TRANSITION, STATE, RESOURCELOCK };
