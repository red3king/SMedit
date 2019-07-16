#include "gui_op_factory.h"
#include "gui_operation.h" 
#include "zoom_in.h"
#include "zoom_out.h"
#include "move_state.h"


bool GUIOpFactory::maybe_create_gui_op(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(ZoomIn::maybe_create(gui_state, current_events, pref))
        return true;

    if(ZoomOut::maybe_create(gui_state, current_events, pref))
        return true;

    if(MoveState::maybe_create(gui_state, current_events, pref))
        return true;

    return false;
}
