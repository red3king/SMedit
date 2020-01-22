#pragma once

#include <string>
#include <vector>
#include "lib/json.hpp"

#include "entity.h"
#include "py_embedded/run_script.h"


using std::string;
using std::vector;


bool load_csc_from_file(CustomStateClass& custom_state_class, string csc_file);


class CustomTransitionDef
{
    public:
        // Same fields from Transition class
        TransitionType type;
        string event_name;  // only used for EVENT type
        float timeout;      // only for TIMEOUT type
        
    private:
};


class CustomStateClass : public Entity
{
    public:
        CustomStateClass(unsigned int id=0);
        CustomStateClass(json jdata);
        
        json to_json();
        
        string path;
        
        // persisted, so at least the user has the name of the missing state class 
        // file if they accidentally misplace it (they lose the transition defs)
        string name;
        
        // not persisted to json, reloaded from state def python classes on program start
        vector<CustomTransitionDef> transition_defs;
    
    private:
};
