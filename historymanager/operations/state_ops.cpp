#include "state_ops.h"
#include "signals.h"


// Abstract modify
StateChgOperation::StateChgOperation(Machine* machine, State* state)
{
    machine_id = machine->id;
    state_id = state->id;
}


unsigned int StateChgOperation::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    State* state = machine->get_state_by_id(state_id);
    execute_impl(state);
    signals.fire_model_changed(STATE, MODIFY, state_id);
    return state_id;
}


// Create

OpStateCreate::OpStateCreate(Machine* machine, float x, float y)
{
    machine_id = machine->id;
    this->x = x;
    this->y = y;
}


unsigned int OpStateCreate::execute(Project& project)
{
    State* state = new State(project.get_next_id());
    state->name = "new state";
    state->type = CODE;
    state->x = x;
    state->y = y;
    state->w = 100;
    state->h = 100;

    Machine* machine = project.get_machine_by_id(machine_id);
    machine->states.push_back(state);

    signals.fire_model_changed(STATE, CREATE, state->id);
    return state->id;
}


OpStateCreate* OpStateCreate::clone()
{
    return new OpStateCreate(*this);
}


// Delete

OpStateDelete::OpStateDelete(Machine* machine, State* state)
{
    machine_id = machine->id;
    state_id = state->id;
}


unsigned int OpStateDelete::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    
    State* to_delete;
    int i;

    for(i=0; i<machine->states.size(); i++)
    {
        if(machine->states[i]->id == state_id)
        {
            to_delete = machine->states[i];
            break;
        }
    }

    signals.fire_model_changed(STATE, PRE_DELETE, state_id);

    // Unlink attached transitions
    for(int i=0; i<to_delete->incoming_transitions.size(); i++)
    {
        Transition* transition = to_delete->incoming_transitions[i];
        transition->to_state = nullptr;
        transition->update_positions();
    }

    for(int i=0; i<to_delete->outgoing_transitions.size(); i++)
    {
        Transition* transition = to_delete->outgoing_transitions[i];
        transition->from_state = nullptr;
        transition->update_positions();
    }

    delete to_delete;
    machine->states.erase(machine->states.begin() + i);
    return state_id;
}


OpStateDelete* OpStateDelete::clone()
{
    return new OpStateDelete(*this);
}


// Move

OpStateMove::OpStateMove(Machine* machine, State* state, float x, float y)
{
    machine_id = machine->id;
    state_id = state->id;
    this->x = x;
    this->y = y;
}


unsigned int OpStateMove::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    State* state = machine->get_state_by_id(state_id);
    state->x = x;
    state->y = y;
    state->update_transition_positions();
    return state->id;
}


bool OpStateMove::may_collapse_impl(Operation& other)
{
    OpStateMove& other_move = dynamic_cast<OpStateMove&>(other);
    return other_move.state_id == state_id;
}


void OpStateMove::collapse(Operation& other)
{
    OpStateMove& other_move = dynamic_cast<OpStateMove&>(other);
    x = other_move.x;
    y = other_move.y;
}


OpStateMove* OpStateMove::clone()
{
    return new OpStateMove(*this);
}


// Type change

OpStateType::OpStateType(Machine* machine, State* state, StateType type) : StateChgOperation(machine, state)
{
    this->type = type;
}


OpStateType* OpStateType::clone()
{
    return new OpStateType(*this);
}


void OpStateType::execute_impl(State* state)
{
    state->type = type;
}


// Name change

OpStateName::OpStateName(Machine* machine, State* state, string name) : StateChgOperation(machine, state)
{
    this->name = name;
}


OpStateName* OpStateName::clone()
{
    return new OpStateName(*this);
}


void OpStateName::execute_impl(State* state)
{
    state->name = name;
}


// Code change

OpStateCode::OpStateCode(Machine* machine, State* state, string code) : StateChgOperation(machine, state)
{
    this->code = code;
}


OpStateCode* OpStateCode::clone()
{
    return new OpStateCode(*this);
}


void OpStateCode::execute_impl(State* state)
{
    state->code = code;
}


