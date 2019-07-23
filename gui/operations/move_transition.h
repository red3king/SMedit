#pragma once

#include "gui_operation.h"
#include "gui/models/gui_transition.h"
#include "gui/gui_state.h"


class MoveTransition : public GUIOperation
{
    public:
        MoveTransition(GMTransition* transition_gmodel, float offset_x, float offset_y);

        static bool maybe_create(GMTransition* transition_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        float offset_x, offset_y;
        GMTransition* transition_gmodel;

};
