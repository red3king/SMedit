#pragma once

#include "gui_box_model.h"


class GMState : public GMBox
{
    public:
        GMState(DrawContext* ctx, State* state);
        
        void get_coords(float& x, float& y, float& w, float& h);
        void set_coords(float x, float y, float w, float h);
        void draw_interior(float x, float y, float w, float h);

        Entity* get_entity();

        NVGcolor get_border_color();
        NVGcolor get_title_text_color();
        std::string get_title();

        State* state;
}; 
