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
    signals.fire_gui_signal(TRANSITION, CREATE, new_transition->id);
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

    signals.fire_gui_signal(TRANSITION, PRE_DELETE, transition_id);
    delete transition;
    machine->transitions.erase(machine->transitions.begin()+i);
    return transition_id;
}


OpTransitionDelete* OpTransitionDelete::clone()
{
    return new OpTransitionDelete(*this);
}
