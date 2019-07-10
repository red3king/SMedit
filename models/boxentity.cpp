#include "boxentity.h"
#include "utils.h"


BoxEntity::BoxEntity(unsigned int id) : Entity(id) 
{

}

/*
 * TODO delete / move to guimodel
bool BoxEntity::mouse_within(int mouse_x, int mouse_y)
{
    return point_in_box(mouse_x, mouse_y, x + BE_INT_GAP, y + BE_INT_GAP, x + w - BE_INT_GAP, y + h - BE_INT_GAP);
}


bool BoxEntity::mouse_on_border(int mouse_x, int mouse_y)
{
    return point_in_box(mouse_x, mouse_y, x - BE_INT_GAP, y - BE_INT_GAP, 
            x + w + BE_INT_GAP, y + h + BE_INT_GAP) && !mouse_within(mouse_x, mouse_y);
};
*/
