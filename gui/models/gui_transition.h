#pragma once

#include "gui_model.h"


class GMTransition : public GUIModel
{
    public:
        GMTransition(DrawContext* ctx, Transition* transition);

        void draw();
        bool mouse_in_range(float mouse_x, float mouse_y);
        Entity* get_entity();

        Transition* transition;
};
