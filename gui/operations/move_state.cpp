#include "move_state.h"


static bool maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_M_MOVE && current_events.pressed_buttons.find(MB_LEFT))
    {
        for(int i=0; i<gui_state.gui_models.size(); i++)
        {
            auto model = gui_state.gui_models[i];
            
            if(model->type != STATE)
                continue;

            State
        }
    }

    return false;
}
