#pragma once

#include "gui_operation.h"
#include "gui/models/gui_resourcelock.h"
#include "gui/gui_state.h"


class MoveResourceLock : public GUIOperation
{
    public:
        MoveResourceLock(GMResourceLock* rlock_gmodel, float offset_x, float offset_y);

        static bool maybe_create(GMResourceLock* rlock_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        float offset_x, offset_y;
        GMResourceLock* rlock_gmodel;
};
