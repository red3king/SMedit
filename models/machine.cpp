#include "machine.h"
#include "state.h"
#include "transition.h"


Machine::Machine(unsigned int id) : Entity(id) { }


Machine::Machine(const Machine& other)
{
    _copy_from(other);
}


Machine& Machine::operator=(const Machine& other)
{
    _copy_from(other);
    return *this;
}
 

void Machine::_copy_from(const Machine& other)
{
    name = other.name;
    run_on_start = other.run_on_start;
    
    states = vector<State*>(other.states.size());
    transitions = vector<Transition*>(other.transitions.size());
    resourcelocks = vector<ResourceLock*>(other.resourcelocks.size());

    for(int i=0; i<other.states.size(); i++)
        states.push_back(new State(*other.states[i]));

    for(int i=0; i<other.transitions.size(); i++)
    {
        Transition* copy_from = other.transitions[i];
        Transition* transition = new Transition(*copy_from);
        transition->from_state = get_state_by_id(copy_from->from_state->id);
        transition->to_state = get_state_by_id(copy_from->to_state->id);
        transitions.push_back(transition); 
    }

    for(int i=0; i<other.resourcelocks.size(); i++)
        resourcelocks.push_back(new ResourceLock(*other.resourcelocks[i]));
}


Machine::~Machine()
{
    for(int i=0; i<resourcelocks.size(); i++)
        delete resourcelocks[i];
    
    for(int i=0; i<transitions.size(); i++)
        delete transitions[i];

    for(int i=0; i<states.size(); i++)
        delete states[i];
}


State* Machine::get_state_by_id(unsigned int id)
{
    for(int i=0; i<states.size(); i++)
    {
        if(states[i]->id == id)
            return states[i];
    }

    return nullptr;
}


Transition* Machine::get_transition_by_id(unsigned int id)
{
    for(int i=0; i<transitions.size(); i++)
    {
        if(transitions[i]->id == id)
            return transitions[i];
    }

    return nullptr;
}


ResourceLock* Machine::get_resourcelock_by_id(unsigned int id)
{
    for(int i=0; i<resourcelocks.size(); i++)
    {
        if(resourcelocks[i]->id == id)
            return resourcelocks[i];
    }

    return nullptr;
}


Entity* Machine::get_entity_by_id(unsigned int id)
{
    Entity* result = get_state_by_id(id);
    
    if(result == nullptr)
        result = get_transition_by_id(id);
    
    if(result == nullptr)
        result = get_resourcelock_by_id(id);

    return result;
}
