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

        virtual CursorType update_impl(CurrentEvents& current_events)=0;
        virtual bool mouse_within(float mouse_x, float mouse_y)=0;
                
        CursorType update(CurrentEvents& current_events, GUIModel*& just_selected);
        unsigned int get_id();

        void set_deselected();

        EntityType type;

    protected:
        DrawContext* ctx;
        bool selected, mouse_over;
};

