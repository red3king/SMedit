#pragma once

#include <string>
#include <vector>
#include "lib/json.hpp"

#include "const.h"
#include "entity.h"
#include "custom_config.h"


using std::string;
using std::vector;


class CustomStateClass;
bool load_csc_from_file(CustomStateClass& custom_state_class, string csc_file, string& error_msg);


class CustomTransitionDef
{
    public:
        CustomTransitionDef(json jdata);
        
        // Same fields from Transition class
        TransitionType type;
        string event_name;  // only used for EVENT type
        float timeout;      // only for TIMEOUT type
        
        json to_json();
    private:
};


class CustomStateClass : public Entity
{
    public:
        CustomStateClass(unsigned int id = 0);
        CustomStateClass(json jdata);
        
        json to_json();
        
        string path;
        
        // persisted, so at least the user has the name of the missing state class 
        // file if they accidentally misplace it (they lose the transition defs)
        string name;
        
        bool has_config();
        string get_py_filename();
        
        EntityType get_entity_type();
        
        // persisted to json, may be reloaded from state def python classes
        vector<CustomTransitionDef> transition_defs;
        vector<CustomConfigDef> configuration;
        
    private:
};
