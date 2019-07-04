#pragma once

#include <vector>

#include "machine.h"
#include "resource.h"

using std::vector;


class Project
{
    public:
        Project(unsigned int id_ctr=0);

        vector<Resource> resources;
        vector<Machine> machines;  

        unsigned int get_next_id();

        // get index into machines vector by id
        int get_mindex_by_id(unsigned int id);

        // " " for resources
        int get_rindex_by_id(unsigned int id);

    private:
        unsigned int id_ctr;
};


