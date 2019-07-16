#pragma once

#include "gui/current_events.h"
#include "gui/gui_state.h"
#include "gui/operations/gui_operation.h"


class GUIOpFactory
{
    public:
        static bool maybe_create_gui_op(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);
};
