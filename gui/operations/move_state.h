#pragma once

#include "gui_operation.h"
#include "gui/models/gui_state.h"
#include "gui/gui_state.h"


class MoveState : public GUIOperation
{
    public:
        MoveState(GMState* state_gmodel, float offset_x, float offset_y);

        static bool maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        float offset_x, offset_y;
        GMState* state_gmodel;
};
