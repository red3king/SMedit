#pragma once

#include "gui_model.h"


class GMTransition : public GUIModel
{
    public:
        GMTransition(DrawContext* ctx, Transition* transition);

        void draw();
        CursorType update_impl(CurrentEvents& current_events);
        bool mouse_within(float mouse_x, float mouse_y);
        Entity* get_entity();

        Transition* transition;

    private:
        void interp(float t, float& x_out, float& y_out);
};
