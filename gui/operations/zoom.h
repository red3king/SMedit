#pragma once

#include "gui_operation.h"
#include "gui/gui_state.h"


class Zoom : public GUIOperation
{
    public:
        static bool maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        Zoom(float zoom_amount, float mouse_x, float mouse_y);

        bool on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        float zoom_amount, mouse_x, mouse_y;
};


