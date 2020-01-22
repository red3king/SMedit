#pragma once

#include <vector>
#include <map>

#include "lib/json.hpp"

#include "machine.h"
#include "resource.h"
#include "custom_state_class.h"

using std::vector;
using std::map;
using nlohmann::json;


class Project
{
    public:
        Project(unsigned int id_ctr=0);
        Project(const Project& other);
        Project(json jdata);

        Project& operator=(const Project& other);
        ~Project();

        json to_json();

        vector<CustomStateClass*> custom_state_classes;
        vector<Resource*> resources;
        vector<Machine*> machines;  
        
        unsigned int get_next_id();

        Machine *get_machine_by_id(unsigned int id);
        int get_mindex_by_id(unsigned int id);

        Resource *get_resource_by_id(unsigned int id);
        int get_rindex_by_id(unsigned int id);
        
        CustomStateClass *get_custom_state_class_by_id(unsigned int id);
        int get_cindex_by_id(unsigned int id);

        Entity *get_entity_by_id(unsigned int id);  //inefficient

    private:
        unsigned int id_ctr;
        void _copy_from(const Project& other);
        void fix_machine_pointers(Machine *other_machine, Machine *new_machine);
        void fix_machine_pointers(json other_json, Machine *new_machine);
        void fix_transition_pointers(Machine *new_machine);
};


