#include "lib/json.hpp"
#include "py_embedded/run_script.h"
#include "custom_state_class.h"


///////  CustomTransitionDef   /////////////

CustomTransitionDef::CustomTransitionDef(json jdata)
{
    type = (TransitionType) jdata["type"];
    event_name = jdata["event_name"];
    timeout = jdata["timeout"];
}


json CustomTransitionDef::to_json()
{
    return { 
        { "type", (int) type }, 
        { "event_name", event_name },
        { "timeout", timeout }
    };
}



///////  CustomStateClass   /////////////

CustomStateClass::CustomStateClass(unsigned int id) : Entity(id) {}


CustomStateClass::CustomStateClass(json jdata) : Entity(jdata)
{
    name = jdata["name"];
    
   // if(jdata.find("path") != jdata.end())     idk what this is
        path = jdata["path"];
    
    auto defs = jdata["outgoing_transitions"];
    
    for(int i = 0; i < defs.size(); i++)
    {
        auto def = defs[i];
        auto new_def = CustomTransitionDef(def);
        transition_defs.push_back(new_def);
    }
    
    auto configs = jdata["configuration_definition"];
    for(int i = 0; i < configs.size(); i++)
    {
        auto cfg = configs[i];
        auto cfg_obj = CustomConfigDef(cfg);
        configuration.push_back(cfg_obj);
    }
}


json CustomStateClass::to_json()
{
    json result = Entity::to_json();
    json outgoing_transitions = json::array();
    json config_def = json::array();
    
    for(int i = 0; i < transition_defs.size(); i++)
    {
        auto json_def = transition_defs[i].to_json();
        outgoing_transitions.push_back(json_def);
    }
    
    for(int i = 0; i < configuration.size(); i++)
    {
        auto json_config = configuration[i].to_json();
        config_def.push_back(json_config);
    }
    
    result["name"] = name;
    result["path"] = path;    
    result["outgoing_transitions"] = outgoing_transitions;
    result["configuration_definition"] = config_def;
    
    return result;
}


bool CustomStateClass::has_config()
{
    return configuration.size() > 0;
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
    custom_state_class.configuration.clear();
    
    for(int i = 0; i < jdata["outgoing_transitions"].size(); i++)
    {
        auto transition = jdata["outgoing_transitions"][i];
        auto def = CustomTransitionDef(transition);
        custom_state_class.transition_defs.push_back(def);
    }
    
    for(int i = 0; i < jdata["configuration_definition"].size(); i++)
    {
        auto config = jdata["configuration_definition"][i];
        auto def = CustomConfigDef(config);
        custom_state_class.configuration.push_back(def);
    }
    
    error_msg = "";
    return true;
}


string CustomStateClass::get_py_filename()
{
    return "csc_" + std::to_string(id) + ".py";
}
