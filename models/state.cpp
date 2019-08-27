#include "state.h"
#include "transition.h"


string state_type_to_string(StateType type)
{
    switch(type)
    {
        case INITIAL:
            return STS_INITIAL;
        case CODE:
            return STS_CODE;
        case RETURN:
            return STS_RETURN;
        case SPAWN:
            return STS_SPAWN;
        case JOIN:
            return STS_JOIN;
    }

    throw std::invalid_argument("unknown StateType");
}


StateType string_to_state_type(string input)
{
    if(input == STS_INITIAL)
        return INITIAL;
    if(input == STS_CODE)
        return CODE;
    if(input == STS_RETURN)
        return RETURN;
    if(input == STS_SPAWN)
        return SPAWN;
    if(input == STS_JOIN)
        return JOIN;

    throw std::invalid_argument("unknown StateType string");
}



State::State(unsigned int id) : BoxEntity(id) 
{ 
    launch_task_name = LVOV("");
    has_return_value = false;
} 


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
