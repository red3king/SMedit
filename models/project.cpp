#include "project.h"


Project::Project(unsigned int id_ctr)
{
    this->id_ctr = id_ctr;
}


Project::Project(const Project& other)
{
    _copy_from(other);
}


Project::Project(json jdata)
{
    id_ctr = jdata["id_ctr"];

    for(int i=0; i<jdata["resources"].size(); i++)
        resources.push_back(new Resource(jdata["resources"][i]));

    for(int i=0; i<jdata["custom_state_classes"].size(); i++)
        custom_state_classes.push_back(new CustomStateClass(jdata["custom_state_classes"][i]));
    
    for(int i=0; i<jdata["machines"].size(); i++)
    {
        json json_machine = jdata["machines"][i];
        auto new_machine = new Machine(json_machine);
        fix_machine_pointers(json_machine, new_machine);
        machines.push_back(new_machine); 
    }
}


Project& Project::operator=(const Project& other)
{
    _copy_from(other);
    return *this;
}


void Project::_copy_from(const Project& other)
{
    resources = vector<Resource*>();
    machines = vector<Machine*>();
    custom_state_classes = vector<CustomStateClass*>();
    
    id_ctr = other.id_ctr;

    for(int i=0; i<other.resources.size(); i++)
        resources.push_back(new Resource(*other.resources[i]));
    
    for(int i=0; i<other.custom_state_classes.size(); i++)
        custom_state_classes.push_back(new CustomStateClass(*other.custom_state_classes[i]));
    
    for(int i=0; i<other.machines.size(); i++)
    {
        auto other_machine = other.machines[i];
        auto new_machine = new Machine(*other_machine);
        fix_machine_pointers(other_machine, new_machine);
        machines.push_back(new_machine);
    }
}


void Project::fix_machine_pointers(Machine* other_machine, Machine* new_machine)
{
    for(int i=0; i<other_machine->transitions.size(); i++)
    {
        Transition* new_transition = new_machine->transitions[i];
        Transition* old_transition = other_machine->transitions[i];
        auto from_id = old_transition->from_state == nullptr ? 0 : old_transition->from_state->id;
        auto to_id = old_transition->to_state == nullptr ? 0 : old_transition->to_state->id;
        new_transition->from_state = new_machine->get_state_by_id(from_id);
        new_transition->to_state = new_machine->get_state_by_id(to_id);
    }

    for(int i=0; i<other_machine->resourcelocks.size(); i++)
    {
        ResourceLock* new_resourcelock = new_machine->resourcelocks[i];
        ResourceLock* other_resourcelock = other_machine->resourcelocks[i];
        auto resource_id = other_resourcelock->resource->id;
        new_resourcelock->resource = get_resource_by_id(resource_id);
    }

    fix_transition_pointers(new_machine);
}


void Project::fix_machine_pointers(json other_json, Machine* new_machine)
{
    for(int i=0; i<other_json["transitions"].size(); i++)
    {   
        Transition* new_transition = new_machine->transitions[i];
        json json_transition = other_json["transitions"][i];
        new_transition->from_state = new_machine->get_state_by_id(json_transition["from_state"]);
        new_transition->to_state = new_machine->get_state_by_id(json_transition["to_state"]);
    }

    for(int i=0; i<other_json["resourcelocks"].size(); i++)
    {
        ResourceLock* new_resourcelock = new_machine->resourcelocks[i];
        json json_resourcelock = other_json["resourcelocks"][i];
        new_resourcelock->resource = get_resource_by_id(json_resourcelock["resource"]);
    }

    fix_transition_pointers(new_machine);
}


void Project::fix_transition_pointers(Machine* new_machine)
{
    for(int i=0; i<new_machine->transitions.size(); i++)
    {
        Transition* transition = new_machine->transitions[i];
        State* to_state = transition->to_state;
        State* from_state = transition->from_state;

        if(to_state != nullptr)
            to_state->add_transition(transition, true);

        if(from_state != nullptr)
            from_state->add_transition(transition, false);
    }
}


Project::~Project()
{
    int i;
    for(i=0; i<machines.size(); i++)
        delete machines[i];

    for(i=0; i<resources.size(); i++)
        delete resources[i];
    
    for(i=0; i<custom_state_classes.size(); i++)
        delete custom_state_classes[i];
}


json Project::to_json()
{
    json jdata = {
        { "id_ctr", id_ctr },
        { "resources", json::array() },
        { "machines", json::array() },
        { "custom_state_classes", json::array() }
    };

    for(int i=0; i<resources.size(); i++)
        jdata["resources"].push_back(resources[i]->to_json());

    for(int i=0; i<machines.size(); i++)
        jdata["machines"].push_back(machines[i]->to_json());
    
    for(int i=0; i<custom_state_classes.size(); i++)
        jdata["custom_state_classes"].push_back(custom_state_classes[i]->to_json);

    return jdata;
}


unsigned int Project::get_next_id()
{
    return ++id_ctr;
}


Machine* Project::get_machine_by_id(unsigned int id)
{
    for(int j=0; j<machines.size(); j++)
    {
        if(machines[j]->id == id)
            return machines[j];
    }

    return nullptr;
}


int Project::get_mindex_by_id(unsigned int id)
{
    for(int j=0; j<machines.size(); j++)
    {
        if(machines[j]->id == id)
            return j;
    }

    return -1;
}


Resource *Project::get_resource_by_id(unsigned int id)
{
    for(int j=0; j<resources.size(); j++)
    {
        if(resources[j]->id == id)
            return resources[j];
    }

    return nullptr;
}


CustomStateClass *Project::get_custom_state_class_by_id(unsigned int id)
{
    for(int i=0; i<custom_state_classes.size(); i++)
    {
        if(custom_state_classes[i]->id == id)
            return custom_state_classes[i];
    }
    
    return nullptr;
}


int Project::get_cindex_by_id(unsigned int id)
{
    for(int i=0; i<custom_state_classes.size(); i++)
    {
        if(custom_state_classes[i]->id == id)
            return i;
    }
    
    return -1;
}


int Project::get_rindex_by_id(unsigned int id)
{
    for(int j=0; j<resources.size(); j++)
    {
        if(resources[j]->id == id)
            return j;
    }

    return -1;
}


Entity *Project::get_entity_by_id(unsigned int id)
{
    Entity *result = get_machine_by_id(id);

    if(result == nullptr)
        result = get_resource_by_id(id);

    if(result == nullptr)
        result = get_custom_state_class_by_id(id);
    
    int i=0;

    while(result == nullptr && i < machines.size())
        result = machines[i++]->get_entity_by_id(id);
    
    return result;
}
