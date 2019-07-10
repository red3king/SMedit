#include "zoom_out.h"


static bool ZoomOut::maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_SCROLL && current_events.mouse_scroll == SC_DOWN)
    {
        pref = new ZoomOut();
        return true;
    }
    return false;
}


void ZoomOut::on_end(Project& current_project, GUIState& gui_state)
{
    gui_state.draw_context.zoom_out();
}
