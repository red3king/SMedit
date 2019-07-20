#pragma once

#include "gui_model.h"


class GMTransition : public GUIModel
{
    public:
        GMTransition(DrawContext* ctx, Transition* transition);

        void draw();
        CursorType update(CurrentEvents& current_events);
        Entity* get_entity();

        Transition* transition;
};
