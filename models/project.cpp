#include "project.h"


Project::Project(unsigned int id_ctr)
{
    this->id_ctr = id_ctr;
}


Project::Project(const Project& other)
{
    _copy_from(other);
}


Project& Project::operator=(const Project& other)
{
    _copy_from(other);
    return *this;
}


void Project::_copy_from(const Project& other)
{
    resources = vector<Resource*>(other.resources.size());
    machines = vector<Machine*>(other.machines.size());
    id_ctr = other.id_ctr;

    for(int i=0; i<other.resources.size(); i++)
        resources.push_back(new Resource(*other.resources[i]));
    
    for(int i=0; i<other.machines.size(); i++)
        machines.push_back(new Machine(*other.machines[i]));
}


Project::~Project()
{
    int i;
    for(i=0; i<machines.size(); i++)
        delete machines[i];

    for(i=0; i<resources.size(); i++)
        delete resources[i];
}


unsigned int Project::get_next_id()
{
    return ++id_ctr;
}


Machine* Project::get_machine_by_id(unsigned int id)
{
    int i = -1;
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


Resource* Project::get_resource_by_id(unsigned int id)
{
    int i = -1;
    for(int j=0; j<resources.size(); j++)
    {
        if(resources[j]->id == id)
            return resources[j];
    }

    return nullptr;
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


Entity* Project::get_entity_by_id(unsigned int id)
{
    Entity* result = get_machine_by_id(id);

    if(result == nullptr)
        result = get_resource_by_id(id);

    int i=0;

    while(result == nullptr && i < machines.size())
        result = machines[i++]->get_entity_by_id(id);
    
    return result;
}
