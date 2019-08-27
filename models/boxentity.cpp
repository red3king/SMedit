#include "boxentity.h"
#include "utils.h"


BoxEntity::BoxEntity(unsigned int id) : Entity(id) 
{

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
