#pragma once

#include "gui_operation.h"
#include "gui/gui_state.h"


class ZoomIn : public GUIOperation
{
    public:
        static bool maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        bool on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);
};


