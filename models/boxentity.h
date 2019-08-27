#pragma once

#include "entity.h"
#include "const.h"

#define MIN_BE_HEIGHT 60
#define MIN_BE_WIDTH 80


class BoxEntity : public Entity
{
    public:
        BoxEntity(unsigned int id=0);
        float x, y, w, h;

        bool new_border_valid(BorderType border, float world_mouse_position);

    private:  
};
