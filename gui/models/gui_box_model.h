#pragma once

#include "gui_model.h"

#define BM_INT_GAP 4


class GMBox : public GUIModel
{
    public:
        GMBox(DrawContext* ctx, EntityType type);
        virtual void get_coords(float& x, float& y, float& w, float& h)=0;
        virtual void set_coords(float x, float y, float w, float h)=0;
        virtual void draw_interior()=0;
        
        void draw();
        bool mouse_in_range(float mouse_x, float mouse_y);

        bool mouse_within(float mouse_x, float mouse_y);
        bool mouse_on_border(float mouse_x, float mouse_y);
};

