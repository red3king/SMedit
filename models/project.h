#pragma once

#include <vector>

#include "machine.h"
#include "resource.h"

using std::vector;


class Project
{
    public:
        Project(unsigned int id_ctr=0);
        Project(const Project& other);
        Project& operator=(const Project& other);
        ~Project();

        vector<Resource*> resources;
        vector<Machine*> machines;  

        unsigned int get_next_id();

        Machine* get_machine_by_id(unsigned int id);
        int get_mindex_by_id(unsigned int id);

        Resource* get_resource_by_id(unsigned int id);
        int get_rindex_by_id(unsigned int id);

        Entity* get_entity_by_id(unsigned int id);  //inefficient

    private:
        unsigned int id_ctr;
        void _copy_from(const Project& other);
};


