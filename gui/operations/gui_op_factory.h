#pragma once
#include "gui/current_events.h"


class GUIOpFactory
{
    public:
        static bool maybe_create_gui_op(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GuiOperation*& pref);
};
