#pragma once

#include "gui_operation.h"


class Move : public GUIOperation
{
    public:
        Move(float initial_x, float initial_y);

        static bool maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        float initial_x, initial_y;
};
