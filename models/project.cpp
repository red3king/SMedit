#include <cassert>

#include "project.h"


Project::Project(unsigned int id_ctr)
{
    this->id_ctr = id_ctr;
}


unsigned int Project::get_next_id()
{
    return ++id_ctr;
}


int Project::get_mindex_by_id(unsigned int id)
{
    int i = -1;
    for(int j=0; j<machines.size(); j++)
    {
        if(machines[j].id == id)
        {
            i = j;
            break;
        }
    }

    assert(i != -1);
    return i;
}


int Project::get_rindex_by_id(unsigned int id)
{
    int i = -1;
    for(int j=0; j<resources.size(); j++)
    {
        if(resources[j].id == id)
        {
            i = j;
            break;
        }
    }

    assert(i != -1);
    return i;
}


