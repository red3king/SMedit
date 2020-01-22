#include "move_transition_endpoint.h"
#include "gui/models/gui_state.h"
#include "historymanager/operations/transition_ops.h"


MoveTransitionEndpoint::MoveTransitionEndpoint(GMTransition* transition_gmodel, bool is_endpoint_0)
{
    this->transition_gmodel = transition_gmodel;
    this->is_endpoint_0 = is_endpoint_0;
    connect_state = nullptr;
}


bool MoveTransitionEndpoint::maybe_create(GMTransition* transition_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type != ET_MB_PRESS)
        return false;

    if(!current_events.button_pressed(MB_LEFT))
        return false;

    TransitionBorder border = transition_gmodel->mouse_on_border(current_events.mouse_x, current_events.mouse_y);

    if(border == TB_NONE || border == TB_MID)
        return false;

    // Can't un-attach child transitions from their parent state
    if(border == TB_BEGIN && transition_gmodel->is_child_transition())
        return false;
    
    pref = new MoveTransitionEndpoint(transition_gmodel, border == TB_BEGIN);
    return true;
}


bool MoveTransitionEndpoint::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float x_new, y_new;
    gui_state.draw_context.screen_to_world(x_new, y_new, current_events.mouse_x, current_events.mouse_y);
    Machine* machine = gui_state.current_machine;
    Transition* transition = transition_gmodel->transition;

    op = new OpTransitionEndpointMove(machine, transition, is_endpoint_0, x_new, y_new);
    return true;
}


bool MoveTransitionEndpoint::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    if(connect_state == nullptr)
        return false;

    Machine* machine = gui_state.current_machine;
    Transition* transition = transition_gmodel->transition;

    op = new OpTransitionEndpointMove(machine, transition, is_endpoint_0, connect_state);
    return true;
}


GUIOpResult MoveTransitionEndpoint::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    if(current_events.button_pressed(MB_LEFT))
        return CONTINUE;

    // If user lets go on a state, save pointer to connect
    for(int i=0; i<gui_state.gui_models.size(); i++)
    {
        GMState* state_gmodel = dynamic_cast<GMState*>(gui_state.gui_models[i]);
        if(state_gmodel == nullptr)
            continue;

        if(state_gmodel->mouse_within(current_events.mouse_x, current_events.mouse_y))
        {
            // Can't attach outgoing transitions to custom states
            if(state_gmodel->is_custom() && is_endpoint_0)
                continue;
            
            connect_state = state_gmodel->state;
        }
    }

    return END;
}
