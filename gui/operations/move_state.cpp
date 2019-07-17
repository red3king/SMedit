#include "move_state.h"
#include "historymanager/operations/state_ops.h"


MoveState::MoveState(GMState* state_gmodel, float offset_x, float offset_y)
{
    this->offset_x = offset_x;
    this->offset_y = offset_y;
    this->state_gmodel = state_gmodel;
}


bool MoveState::maybe_create(GMState* state, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type != ET_MB_PRESS)
        return false;
    
    float x, y, w, h;
    state->get_coords(x, y, w, h);

    if(state->mouse_within(current_events.mouse_x, current_events.mouse_y))
    {
        float mouse_x_world, mouse_y_world;
        gui_state.draw_context.screen_to_world(mouse_x_world, mouse_y_world, current_events.mouse_x, current_events.mouse_y);
        float offset_x = mouse_x_world - x;
        float offset_y = mouse_y_world - y;
        pref = new MoveState(state, offset_x, offset_y);
        return true;
    }

    return false;
}


GUIOpResult MoveState::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    if(!current_events.button_pressed(MB_LEFT))
        return END;

    return CONTINUE;
}


bool MoveState::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float mouse_x_world, mouse_y_world;
    gui_state.draw_context.screen_to_world(mouse_x_world, mouse_y_world, current_events.mouse_x, current_events.mouse_y);
    float new_x = mouse_x_world - offset_x;
    float new_y = mouse_y_world - offset_y;
    op = new OpStateMove(gui_state.current_machine, state_gmodel->state, new_x, new_y);
    return true;
}



