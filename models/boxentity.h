#pragma once

#include "entity.h"


// TODO remove #define BE_INT_GAP 4

class BoxEntity : public Entity
{
    public:
        BoxEntity(unsigned int id=0);
        float x, y, w, h;

    private:  
};
