#include "machine.h"
#include "state.h"
#include "transition.h"
#include "project.h"


Machine::Machine(unsigned int id) : Entity(id) { }


Machine::Machine(const Machine& other) : Entity(other)
{
    // copies fields and sub objects but does not assign their pointers
    name = other.name;
    run_on_start = other.run_on_start;
    
    states = vector<State*>();
    transitions = vector<Transition*>();
    resourcelocks = vector<ResourceLock*>();

    for(int i=0; i<other.states.size(); i++)
        states.push_back(new State(*other.states[i]));

    for(int i=0; i<other.transitions.size(); i++)
    {
        Transition* transition = new Transition(*other.transitions[i]);
        transitions.push_back(transition); 
    }

    for(int i=0; i<other.resourcelocks.size(); i++)
    {
        auto new_lock = new ResourceLock(*other.resourcelocks[i]);
        resourcelocks.push_back(new_lock);
    }
}


Machine::Machine(json jdata) : Entity(jdata)
{
    name = jdata["name"];
    run_on_start = jdata["run_on_start"];

    for(int i=0; i<jdata["states"].size(); i++)
        states.push_back(new State(jdata["states"][i]));

    for(int i=0; i<jdata["transitions"].size(); i++)
        transitions.push_back(new Transition(jdata["transitions"][i]));

    for(int i=0; i<jdata["resourcelocks"].size(); i++)
        resourcelocks.push_back(new ResourceLock(jdata["resourcelocks"][i]));
}

        
json Machine::to_json()
{
    json jdata = Entity::to_json();
    jdata["name"] = name;
    jdata["run_on_start"] = run_on_start;

    jdata["states"] = json::array();
    jdata["transitions"] = json::array();
    jdata["resourcelocks"] = json::array();

    for(int i=0; i<states.size(); i++)
        jdata["states"].push_back(states[i]->to_json());

    for(int i=0; i<transitions.size(); i++)
        jdata["transitions"].push_back(transitions[i]->to_json());

    for(int i=0; i<resourcelocks.size(); i++)
        jdata["resourcelocks"].push_back(resourcelocks[i]->to_json());

    return jdata;
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
