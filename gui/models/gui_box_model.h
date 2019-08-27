#pragma once

#include "gui_model.h"
#include "historymanager/operations/boxentity_ops.h"

#define BM_INT_GAP 3


class GMBox : public GUIModel
{
    public:
        GMBox(DrawContext* ctx, EntityType type);

        virtual void draw_interior(float x, float y, float w, float h)=0;
        virtual NVGcolor get_border_color()=0;
        virtual NVGcolor get_title_text_color()=0;
        virtual std::string get_title()=0;
        virtual bool get_icon(int& icon_image)=0;  // if it returns false, there is no icon.

        void get_coords(float& x, float& y, float& w, float& h); 
        void set_coords(float x, float y, float w, float h);

        CursorType update_impl(CurrentEvents& current_events);
        void draw();

        bool mouse_within(float mouse_x, float mouse_y);
        BorderType mouse_on_border(float mouse_x, float mouse_y);

        bool new_border_valid(BorderType border, float world_mouse_position);

    protected:
        int text_line_no;
        float text_line_pos;
        bool text(string text, NVGcolor color, bool stay=false);  // returns false if there was no room left
};

