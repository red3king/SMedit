#pragma once

#include "entity.h"
#include "const.h"

#define MIN_BE_HEIGHT 60
#define MIN_BE_WIDTH 80


class BoxEntity : public Entity
{
    public:
        BoxEntity(unsigned int id=0);
        BoxEntity(json jdata);

        json to_json();

        float x, y, w, h;
        
        bool new_border_valid(BorderType border, float world_mouse_position);

        void get_center(float& x, float& y);
        
    private:  
};
