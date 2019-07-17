#include "state_ops.h"
#include "signals.h"


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

    signals.fire_gui_signal(STATE, CREATE, state->id);
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

    signals.fire_gui_signal(STATE, PRE_DELETE, state_id);

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

// 