#pragma once

#include "models/entity.h"
#include "models/project.h"
#include "gui/draw_context.h"


class GUIModel
{
    public:
        GUIModel(DrawContext* ctx, EntityType type);
        virtual ~GUIModel()=0;

        virtual void draw()=0;
        virtual bool mouse_in_range(float mouse_x, float mouse_y)=0;
        virtual Entity* get_entity()=0;

        unsigned int get_id();

        EntityType type;

    protected:
        DrawContext* ctx;
};

