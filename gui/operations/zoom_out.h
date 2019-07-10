#pragma once

#include "gui_operation.h"


class ZoomOut : public GUIOperation
{
    public:
        static bool maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        void on_end(Project& current_project, GUIState& gui_state);
};
