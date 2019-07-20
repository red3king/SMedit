#pragma once

#include "models/entity.h"
#include "models/project.h"
#include "gui/draw_context.h"
#include "gui/current_events.h"
#include "signals.h"


class GUIModel
{
    public:
        GUIModel(DrawContext* ctx, EntityType type);
        virtual ~GUIModel()=0;

        virtual void draw()=0;
        virtual Entity* get_entity()=0;
        virtual CursorType update(CurrentEvents& current_events)=0;

        unsigned int get_id();

        EntityType type;

    protected:
        DrawContext* ctx;
};

