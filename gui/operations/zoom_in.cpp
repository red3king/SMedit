#include "zoom_in.h"


static bool ZoomIn::maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_SCROLL && current_events.mouse_scroll == SC_UP)
    {
        pref = new ZoomIn();
        return true;
    }

    return false;
}


void ZoomIn::on_end(Project& current_project, GUIState& gui_state)
{
    gui_state.draw_context.zoom_in();
}

