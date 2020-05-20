#include "transition_ops.h"
#include "signals.h"


// Generic

TransitionOperation::TransitionOperation(Machine* machine, Transition* transition) : MachineRelatedOperation(machine)
{
    if(transition != nullptr)
        transition_id = transition->id;
    else
        transition_id = 0;
}


// Create

OpTransitionCreate::OpTransitionCreate(Machine* machine, float x0, float y0, float x1, float y1) : TransitionOperation(machine, nullptr)
{
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
}


Transition *create_transition(Project& project, unsigned int machine_id, bool is_child, float x0, float y0, float x1, float y1, int type)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* new_transition = new Transition(project.get_next_id());

    if(type >= 0)
        new_transition->type = ((TransitionType)type);
    
    new_transition->x0 = x0;
    new_transition->y0 = y0;
    new_transition->x1 = x1;
    new_transition->y1 = y1;
    new_transition->is_child_transition = is_child;
    
    machine->transitions.push_back(new_transition);
    signals.fire_model_changed(TRANSITION, CREATE, new_transition->id);
    return new_transition;   
}


unsigned int OpTransitionCreate::execute(Project& project)
{
    Transition *new_transition = create_transition(project, machine_id, false, x0, y0, x1, y1, 0);
    return new_transition->id;
}


OpTransitionCreate* OpTransitionCreate::clone()
{
    return new OpTransitionCreate(*this);
}


// Delete

OpTransitionDelete::OpTransitionDelete(Machine* machine, Transition* transition) : TransitionOperation(machine, transition) { }


unsigned int OpTransitionDelete::execute(Project& project)
{
    int i;
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* transition;
    
    for(i = 0; i < machine->transitions.size(); i++)
    {
        transition = machine->transitions[i];
        if(transition->id == transition_id)
            break;
    }

    delete_transition(machine, transition);
    return transition_id;
}


void delete_transition(Machine *machine, Transition *transition)
{
    signals.fire_model_changed(TRANSITION, PRE_DELETE, transition->id);

    // Unlink from to and from states
    if(transition->from_state != nullptr)
        transition->from_state->remove_transition(transition, false);
    
    if(transition->to_state != nullptr)
        transition->to_state->remove_transition(transition, true);

    delete transition;
    
    int tindex = -1;
    while(machine->transitions[++tindex] != transition);
    
    machine->transitions.erase(machine->transitions.begin() + tindex);    
}


OpTransitionDelete* OpTransitionDelete::clone()
{
    return new OpTransitionDelete(*this);
}


// Move

OpTransitionMove::OpTransitionMove(Machine* machine, Transition* transition, float x0_new, float y0_new) : TransitionOperation(machine, transition)
{
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

OpTransitionEndpointMove::OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, float x_new, float y_new) : TransitionOperation(machine, transition)
{
    this->is_endpoint_0 = is_endpoint_0;
    this->x_new = x_new;
    this->y_new = y_new;
    new_state_id = -1;
}


OpTransitionEndpointMove::OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, State* new_state) : TransitionOperation(machine, transition)
{
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
    
    return transition_id;
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

TransitionChgOperation::TransitionChgOperation(Machine* machine, Transition* transition) : TransitionOperation(machine, transition) { }


unsigned int TransitionChgOperation::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    Transition* transition = machine->get_transition_by_id(transition_id);
    execute_impl(transition);
    signals.fire_model_changed(TRANSITION, MODIFY, transition_id);
    return transition_id;
}


// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE

/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.op_specs import *

for line in transition_opgen.make_cpp():
    cog.outl(line)

]]]*/


// type  -  AUTOGENERATED, DO NOT MODIFY

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


bool OpTransitionType::may_collapse_impl(Operation& other)
{
    auto op = (OpTransitionType&) other;
    return transition_id == op.transition_id;
}


void OpTransitionType::collapse(Operation& other)
{
    auto op = (OpTransitionType&) other;
    type = op.type;
}


// timeout  -  AUTOGENERATED, DO NOT MODIFY

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


bool OpTransitionTimeout::may_collapse_impl(Operation& other)
{
    auto op = (OpTransitionTimeout&) other;
    return transition_id == op.transition_id;
}


void OpTransitionTimeout::collapse(Operation& other)
{
    auto op = (OpTransitionTimeout&) other;
    timeout = op.timeout;
}


// event_name  -  AUTOGENERATED, DO NOT MODIFY

OpTransitionEventName::OpTransitionEventName(Machine* machine, Transition* transition, string event_name) : TransitionChgOperation(machine, transition)
{
    this->event_name = event_name;
}


OpTransitionEventName* OpTransitionEventName::clone()
{
    return new OpTransitionEventName(*this);
}


void OpTransitionEventName::execute_impl(Transition* transition)
{
    transition->event_name = event_name;
}


bool OpTransitionEventName::may_collapse_impl(Operation& other)
{
    auto op = (OpTransitionEventName&) other;
    return transition_id == op.transition_id;
}


void OpTransitionEventName::collapse(Operation& other)
{
    auto op = (OpTransitionEventName&) other;
    event_name = op.event_name;
}


//[[[end]]]



