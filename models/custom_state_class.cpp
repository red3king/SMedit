#include "lib/json.hpp"

#include "custom_state_class.h"


CustomStateClass::CustomStateClass(unsigned int id) : Entity(id) {}


CustomStateClass::CustomStateClass(json jdata)
{
    name = jdata["name"];
    path = jdata["path"];
}


json CustomStateClass::to_json()
{
    return { { "name", name }, { "path", path } };
}



bool load_csc_from_file(CustomStateClass& custom_state_class, string csc_file)
{
    // return code indicates success
    ScriptOutput output = PYScripts.run("examine_custom_state_class", csc_file);
    
    if(output.error)
        return false;
    
    string value = output.result;
    json jdata = json::parse(value);
    
    custom_state_class.name = jdata["name"];
    custom_state_class.transition_defs.clear();
    
    for(int i=0; i<jdata["transitions"].size(); i++)
    {
        auto transition = jdata["transitions"];
        auto def = CustomTransitionDef();
        
        def.type = transition["type"];
        def.event_name = transition["event_name"];
        def.timeout = transition["timeout"];
        
        custom_state_class.transition_defs.push_back(def);
    }
}

