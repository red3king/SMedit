#include "transition_ops.h"
#include "signals.h"


// Create

OpTransitionCreate::OpTransitionCreate(Machine* machine, float x0, float y0, float x1, float y1)
{
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
    machine_id = machine->id;
}


unsigned int OpTransitionCreate::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* new_transition = new Transition(project.get_next_id());

    new_transition->x0 = x0;
    new_transition->y0 = y0;
    new_transition->x1 = x1;
    new_transition->y1 = y1;

    machine->transitions.push_back(new_transition);
    signals.fire_model_changed(TRANSITION, CREATE, new_transition->id);
    return new_transition->id;
}


OpTransitionCreate* OpTransitionCreate::clone()
{
    return new OpTransitionCreate(*this);
}


// Delete

OpTransitionDelete::OpTransitionDelete(Machine* machine, Transition* transition)
{
    machine_id = machine->id;
    transition_id = transition->id;
}


unsigned int OpTransitionDelete::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);

    int i;
    Transition* transition;
    for(i=0; i<machine->transitions.size(); i++)
    {
        transition = machine->transitions[i];
        if(transition->id == transition_id)
            break;
    }

    signals.fire_model_changed(TRANSITION, PRE_DELETE, transition_id);

    // Unlink from to and from states
    if(transition->from_state != nullptr)
        transition->from_state->remove_transition(transition, false);
    
    if(transition->to_state != nullptr)
        transition->to_state->remove_transition(transition, true);

    delete transition;
    machine->transitions.erase(machine->transitions.begin()+i);
    return transition_id;
}


OpTransitionDelete* OpTransitionDelete::clone()
{
    return new OpTransitionDelete(*this);
}


// Move

OpTransitionMove::OpTransitionMove(Machine* machine, Transition* transition, float x0_new, float y0_new)
{
    machine_id = machine->id;
    transition_id = transition->id;
    this->x0_new = x0_new;
    this->y0_new = y0_new;
}


OpTransitionMove* OpTransitionMove::clone()
{
    return new OpTransitionMove(*this);
}


unsigned int OpTransitionMove::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* transition = machine->get_transition_by_id(transition_id);

    float dy = transition->y1 - transition->y0;
    float dx = transition->x1 - transition->x0;

    transition->y0 = y0_new;
    transition->x0 = x0_new;
    transition->y1 = y0_new + dy;
    transition->x1 = x0_new + dx;

    return transition_id;
}


bool OpTransitionMove::may_collapse_impl(Operation& other)
{
    OpTransitionMove& other_transition = dynamic_cast<OpTransitionMove&>(other);
    return transition_id == other_transition.transition_id;
}


void OpTransitionMove::collapse(Operation& other)
{
    OpTransitionMove& other_transition = dynamic_cast<OpTransitionMove&>(other);
    x0_new = other_transition.x0_new;
    y0_new = other_transition.y0_new;
}


// Endpoint Move

OpTransitionEndpointMove::OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, float x_new, float y_new)
{
    machine_id = machine->id;
    transition_id = transition->id;
    this->is_endpoint_0 = is_endpoint_0;
    this->x_new = x_new;
    this->y_new = y_new;
    new_state_id = -1;
}


OpTransitionEndpointMove::OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, State* new_state)
{
    machine_id = machine->id;
    transition_id = transition->id;
    this->is_endpoint_0 = is_endpoint_0;
    new_state_id = new_state->id;
    this->x_new = 0;
    this->y_new = 0;
}


OpTransitionEndpointMove* OpTransitionEndpointMove::clone()
{
    return new OpTransitionEndpointMove(*this);
}


unsigned int OpTransitionEndpointMove::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* transition = machine->get_transition_by_id(transition_id);
    
    if(new_state_id == -1)
    {
        if(is_endpoint_0)
        {
            transition->x0 = x_new;
            transition->y0 = y_new;

            if(transition->from_state != nullptr)
                transition->from_state->remove_transition(transition, false);
            
            transition->from_state = nullptr;
    
        }
        else
        {
            transition->x1 = x_new;
            transition->y1 = y_new;
            
            if(transition->to_state != nullptr)
                transition->to_state->remove_transition(transition, true);

            transition->to_state = nullptr;
        }
    }

    else
    {
        State* new_state = machine->get_state_by_id(new_state_id);

        if(is_endpoint_0)
            transition->from_state = new_state;
        else
            transition->to_state = new_state;

        new_state->add_transition(transition, !is_endpoint_0);
    }

    transition->update_positions();
}


bool OpTransitionEndpointMove::may_collapse_impl(Operation& other)
{
    OpTransitionEndpointMove& other_op = dynamic_cast<OpTransitionEndpointMove&>(other);
    return transition_id == other_op.transition_id && is_endpoint_0 == other_op.is_endpoint_0;
}


void OpTransitionEndpointMove::collapse(Operation& other)
{
    OpTransitionEndpointMove& other_op = dynamic_cast<OpTransitionEndpointMove&>(other);
    x_new = other_op.x_new;
    y_new = other_op.y_new;
    new_state_id = other_op.new_state_id;
}


// TransitionChgOperation

TransitionChgOperation::TransitionChgOperation(Machine* machine, Transition* transition)
{
    machine_id = machine->id;
    transition_id = transition->id;
}


unsigned int TransitionChgOperation::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* transition = machine->get_transition_by_id(transition_id);
    execute_impl(transition);
    return transition_id;
}



// Type change

OpTransitionType::OpTransitionType(Machine* machine, Transition* transition, TransitionType type) : TransitionChgOperation(machine, transition)
{
    this->type = type;
}


OpTransitionType* OpTransitionType::clone()
{
    return new OpTransitionType(*this);
}


void OpTransitionType::execute_impl(Transition* transition)
{
    transition->type = type;
}


// Timeout change

OpTransitionTimeout::OpTransitionTimeout(Machine* machine, Transition* transition, float timeout) : TransitionChgOperation(machine, transition)
{
    this->timeout = timeout;
}


OpTransitionTimeout* OpTransitionTimeout::clone()
{
    return new OpTransitionTimeout(*this);
}


void OpTransitionTimeout::execute_impl(Transition* transition)
{
    transition->timeout = timeout;
}


// Event name change

OpTransitionEvName::OpTransitionEvName(Machine* machine, Transition* transition, string event_name) : TransitionChgOperation(machine, transition)
{
    this->event_name = event_name;
}


OpTransitionEvName* OpTransitionEvName::clone()
{
    return new OpTransitionEvName(*this);
}


void OpTransitionEvName::execute_impl(Transition* transition)
{
    transition->event_name = event_name;
}

