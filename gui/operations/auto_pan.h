#pragma once

#include <time.h>

#include "gui_operation.h"
#include "gui/animation_timer.h"


#define AUTO_PAN_SECONDS 1.2


class AutoPan : public GUIOperation
{
    public:
        AutoPan(GUIState& gui_state);

        static bool maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);
        bool on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);
    
    private:
        bool has_last_update;
        time_t last_update_time;
        AnimationCB animation_callback;        
        int cb_id;

        double get_elapsed_seconds(CurrentEvents& current_events);
};
