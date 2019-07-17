#include "move.h"


Move::Move(float initial_x, float initial_y)
{
    this->initial_x = initial_x;
    this->initial_y = initial_y;
}


bool Move::maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type == ET_MB_PRESS && current_events.button_pressed(MB_LEFT))
    {
        pref = new Move(current_events.mouse_x, current_events.mouse_y);
        return true;
    }

    return false;
}


GUIOpResult Move::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    if(current_events.button_pressed(MB_LEFT))
        return CONTINUE;

    return END;
}


bool Move::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float dx, dy, dxw, dyw;
    dx = current_events.mouse_x - initial_x;
    dy = current_events.mouse_y - initial_y;
    dxw = gui_state.draw_context.screen_dist_to_world(dx);
    dyw = gui_state.draw_context.screen_dist_to_world(dy);
    gui_state.draw_context.move(dxw, dyw);
    initial_x += dx;
    initial_y += dy;
    return false;
}
