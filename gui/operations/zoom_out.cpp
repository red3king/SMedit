#include "zoom_out.h"


bool ZoomOut::maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_SCROLL && current_events.mouse_scroll == SC_DOWN)
    {
        pref = new ZoomOut();
        return true;
    }
    return false;
}


bool ZoomOut::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    gui_state.draw_context.zoom_out();
    return false;
}
