#pragma once

#include "gui_model.h"


enum TransitionBorder { TB_NONE, TB_BEGIN, TB_MID, TB_END };


class GMTransition : public GUIModel
{
    public:
        GMTransition(DrawContext* ctx, Transition* transition);

        void draw();
        CursorType update_impl(CurrentEvents& current_events);
        bool mouse_within(float mouse_x, float mouse_y);
        Entity* get_entity();

        Transition* transition;
        TransitionBorder mouse_on_border(float mouse_x, float mouse_y);

    private:
        void interp(float t, float& x_out, float& y_out);
        void calc_arc_params(float& cx, float& cy, float& r, float& a0, float& a1, int& dir); // in world coords
        void draw_as_line();
        void draw_as_arc();

        bool is_arc();
        bool arc_loop_upwards();

        NVGcolor get_main_color();
        NVGcolor get_highlight_color();
};
