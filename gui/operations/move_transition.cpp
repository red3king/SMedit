#include "move_transition.h"
#include "historymanager/operations/transition_ops.h"


MoveTransition::MoveTransition(GMTransition* transition_gmodel, float offset_x, float offset_y)
{
    this->transition_gmodel = transition_gmodel;
    this->offset_x = offset_x;
    this->offset_y = offset_y;
}


bool MoveTransition::maybe_create(GMTransition* transition_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type != ET_MB_PRESS)
        return false;

    if(!current_events.button_pressed(MB_LEFT))
        return false;

    if(transition_gmodel->mouse_on_border(current_events.mouse_x, current_events.mouse_y) != TB_MID)
        return false;

    float x0 = transition_gmodel->transition->x0;
    float y0 = transition_gmodel->transition->y0;
    float mouse_wx, mouse_wy;
    gui_state.draw_context.screen_to_world(mouse_wx, mouse_wy, current_events.mouse_x, current_events.mouse_y);

    pref = new MoveTransition(transition_gmodel, mouse_wx - x0, mouse_wy - y0);
    return true;
}


GUIOpResult MoveTransition::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    return current_events.button_pressed(MB_LEFT) ? CONTINUE : END;
}


bool MoveTransition::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float mouse_wx, mouse_wy;
    gui_state.draw_context.screen_to_world(mouse_wx, mouse_wy, current_events.mouse_x, current_events.mouse_y);
    float x0_new = mouse_wx - offset_x;
    float y0_new = mouse_wy - offset_y;
    Machine* machine = gui_state.current_machine;
    op = new OpTransitionMove(machine, transition_gmodel->transition, x0_new, y0_new);
    return true;
}
