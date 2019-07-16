#pragma once

#include "entity.h"


class BoxEntity : public Entity
{
    public:
        BoxEntity(unsigned int id=0);
        float x, y, w, h;

    private:  
};
