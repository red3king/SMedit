#include "zoom_in.h"


bool ZoomIn::maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_SCROLL && current_events.mouse_scroll == SC_UP)
    {
        pref = new ZoomIn();
        return true;
    }

    return false;
}


bool ZoomIn::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    gui_state.draw_context.zoom_in();
    return false;
}

