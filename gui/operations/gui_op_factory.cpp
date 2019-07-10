#include "gui_op_factory.h"
#include "zoom_in.h"
#include "zoom_out.h"


static bool GUIOpFactory::maybe_create_gui_op(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GuiOperation*& pref)
{
    if(ZoomIn::maybe_create(current_project, gui_state, current_events, pref))
        return true;

    if(ZoomOut::maybe_create(current_project, gui_state, current_events, pref))
        return true;


    return false;
}
