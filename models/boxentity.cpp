#include "boxentity.h"
#include "utils.h"


BoxEntity::BoxEntity(unsigned int id) : Entity(id) 
{
    x = 0;
    y = 0;
    w = 0;
    h = 0;
}


BoxEntity::BoxEntity(json jdata) : Entity(jdata)
{
    x = jdata["x"];
    y = jdata["y"];
    w = jdata["w"]; 
    h = jdata["h"];
}


json BoxEntity::to_json()
{
    json jdata = Entity::to_json();
    jdata["x"] = x;
    jdata["y"] = y;
    jdata["w"] = w;
    jdata["h"] = h;
    return jdata;
}


bool BoxEntity::new_border_valid(BorderType border, float world_mouse_position)
{
    if(border == BT_TOP)
        return MIN_BE_HEIGHT < y + h - world_mouse_position;

    if(border == BT_RIGHT)
        return MIN_BE_WIDTH < world_mouse_position - x;

    if(border == BT_BOTTOM)
        return MIN_BE_HEIGHT < world_mouse_position - y;

    if(border == BT_LEFT)
        return MIN_BE_WIDTH < x + w - world_mouse_position;

    throw std::invalid_argument("BT_NONE not valid here");
}


void BoxEntity::get_center(float& x, float& y)
{
    x = this->x + (w / 2);
    y = this->y + (h / 2);
}

