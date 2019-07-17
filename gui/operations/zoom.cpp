#include "zoom.h"

#include <iostream>


bool Zoom::maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_SCROLL)
    {
        pref = new Zoom(current_events.scroll_amount, current_events.mouse_x, current_events.mouse_y);
        return true;
    }

    return false;
}


Zoom::Zoom(float zoom_amount, float mouse_x, float mouse_y)
{
    this->zoom_amount = zoom_amount;
    this->mouse_x = mouse_x;
    this->mouse_y = mouse_y;
}


bool Zoom::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    gui_state.draw_context.zoom(zoom_amount, mouse_x, mouse_y);
    return false;
}

