#pragma once

#include "gui_operation.h"
#include "gui/models/gui_transition.h"
#include "gui/gui_state.h"


class MoveTransitionEndpoint : public GUIOperation
{
    public:
        MoveTransitionEndpoint(GMTransition* transition_gmodel, bool is_endpoint_0);

        static bool maybe_create(GMTransition* transition_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);
        bool on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        GMTransition* transition_gmodel;
        State* connect_state;
        bool is_endpoint_0;
};
