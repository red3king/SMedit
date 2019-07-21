#pragma once

#include "gui_model.h"
#include "historymanager/operations/boxentity_ops.h"

#define BM_INT_GAP 3


class GMBox : public GUIModel
{
    public:
        GMBox(DrawContext* ctx, EntityType type);

        virtual void get_coords(float& x, float& y, float& w, float& h)=0; // TODO - since i added get_entity(), these dont have to be virtual and i can remove the implementations from subclasses
        virtual void set_coords(float x, float y, float w, float h)=0;
        virtual void draw_interior(float x, float y, float w, float h)=0;
        virtual NVGcolor get_border_color()=0;
        virtual NVGcolor get_title_text_color()=0;
        virtual std::string get_title()=0;

        CursorType update_impl(CurrentEvents& current_events);
        void draw();

        bool mouse_within(float mouse_x, float mouse_y);
        BorderType mouse_on_border(float mouse_x, float mouse_y);
};

