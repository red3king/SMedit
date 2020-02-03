#include "lib/json.hpp"
#include "py_embedded/run_script.h"
#include "custom_state_class.h"


CustomStateClass::CustomStateClass(unsigned int id) : Entity(id) {}


CustomStateClass::CustomStateClass(json jdata) : Entity(jdata)
{
    name = jdata["name"];
    path = jdata["path"];
    
    auto defs = jdata["outgoing_transitions"];
    
    for(int i=0; i<defs.size(); i++)
    {
        auto def = defs[i];
        auto new_def = CustomTransitionDef();
        
        new_def.type = def["type"];
        new_def.event_name = def["event_name"];
        new_def.timeout = def["timeout"];
        
        transition_defs.push_back(new_def);
    }
}


json CustomStateClass::to_json()
{
    json result = Entity::to_json();
    json outgoing_transitions = json::array();
    
    for(int i=0; i<transition_defs.size(); i++)
    {
        auto def = transition_defs[i];
        auto json_def = json::object();
        json_def["type"] = def.type;
        json_def["event_name"] = def.event_name;
        json_def["timeout"] = def.timeout;
        outgoing_transitions.push_back(json_def);
    }
    
    result["name"] = name;
    result["path"] = path;
    result["outgoing_transitions"] = outgoing_transitions;
    
    return result;
}


bool load_csc_from_file(CustomStateClass& custom_state_class, string csc_file, string& error_msg)
{
    // return code indicates success
    ScriptOutput output = scripts.run("examine_custom_state_class", csc_file);
    
    if(output.error)
    {
        error_msg = output.exception_class + ":\r\n" + output.exception_message;
        return false;
    }
    
    string value = std::get<string>(output.result);
    json jdata = json::parse(value);
    
    custom_state_class.name = jdata["name"];
    custom_state_class.transition_defs.clear();
    
    for(int i=0; i<jdata["outgoing_transitions"].size(); i++)
    {
        auto transition = jdata["outgoing_transitions"][i];
        auto def = CustomTransitionDef();
        
        def.type = transition["type"];
        def.event_name = transition["event_name"];
        def.timeout = transition["timeout"];
        
        custom_state_class.transition_defs.push_back(def);
    }
    
    error_msg = "";
    return true;
}

