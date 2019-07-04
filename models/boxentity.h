#pragma once

#include "entity.h"


#define BE_INT_GAP 4

class BoxEntity : public Entity
{
    public:
        BoxEntity(unsigned int id=0);
        int x, y, w, h;

        bool mouse_within(int mouse_x, int mouse_y);    // is mouse hovering inside interior
        bool mouse_on_border(int mouse_x, int mouse_y); // is mouse hovering on the border


    private:  
};
