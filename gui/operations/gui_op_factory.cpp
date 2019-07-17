#include "gui_op_factory.h"
#include "gui_operation.h" 
#include "zoom.h"
#include "move_state.h"


bool GUIOpFactory::maybe_create_gui_op(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(Zoom::maybe_create(gui_state, current_events, pref))
        return true;

    if(MoveState::maybe_create(gui_state, current_events, pref))
        return true;

    return false;
}
