#include "state.h"
#include "transition.h"


State::State(unsigned int id) : BoxEntity(id) { } 


void State::add_transition(Transition* transition, bool incoming)
{
    if(incoming)
        incoming_transitions.push_back(transition);
    else
        outgoing_transitions.push_back(transition);
}


void State::remove_transition(Transition* transition, bool incoming)
{
    auto vec = incoming ? &incoming_transitions : &outgoing_transitions;
    
    int i;
    for(i=0; i<vec->size(); i++)
    {
        if((*vec)[i] == transition)
            break;
    }

    vec->erase(vec->begin() + i);
}


void State::update_transition_positions()
{
    for(int i=0; i<incoming_transitions.size(); i++)
        incoming_transitions[i]->update_positions();

    for(int i=0; i<outgoing_transitions.size(); i++)
        outgoing_transitions[i]->update_positions();
}
