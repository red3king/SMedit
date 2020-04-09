#include "state.h"
#include "transition.h"


bool state_type_is_custom(int type)
{
    return type >= 0;
}


vector<string> find_resources_in_code(string code)
{
    // return list of resource names passed as string literals
    // to get_resource() function
    
    vector<string> result;
    int last = 0;
    string get_resource = "get_resource(";
    
    while(true)
    {
        int pos = code.find(get_resource, last);
        
        if(pos == string::npos)
            break;
        
        int rpos = code.find(")", pos);
        
        if(rpos == string::npos)
            break;
        
        if(rpos - pos - get_resource.length() < 3)
            continue;
        
        char el = code[pos + get_resource.length()];
        
        if(el != '\'' && el != '\"')
            continue;
        
        string resource_name = code.substr(pos + get_resource.length() + 1, rpos - 2 - pos - get_resource.length());
        result.push_back(resource_name);
        
        last = pos + 1;
    }
        
    return result;
}



////// State

bool State::is_custom()
{
    return state_type_is_custom(type);
}


State::State(unsigned int id) : BoxEntity(id) 
{ 
    string empty = "";
    launch_task_name = LVOV(empty);
    has_return_value = false;
    code = DEFAULT_STATE_CODE;
    custom_type = nullptr;
    type = INITIAL;
}


State::State(json jdata) : BoxEntity(jdata), return_value(jdata["return_value"]), launch_task_name(jdata["launch_task_name"])
{
    custom_type = nullptr;
    name = jdata["name"];
    type = (StateType) jdata["type"];
    
    for(int i = 0; i < jdata["initial_args"].size(); i++)
        initial_args.push_back(ArgDef(jdata["initial_args"][i]));

    has_return_value = jdata["has_return_value"];
    launch_synchronous = jdata["launch_synchronous"];
    
    for(int i = 0; i < jdata["launch_args"].size(); i++)
        launch_args.push_back(Arg(jdata["launch_args"][i]));
    
    for(int i = 0; i < jdata["custom_config"].size(); i++)
    {
        auto config = CustomStateConfig(jdata["custom_config"][i]);
        custom_config[config.name] = config;
    }

    launch_result_variable = jdata["launch_result_variable"];
    join_pid_variable = jdata["join_pid_variable"];    
    join_result_variable = jdata["join_result_variable"];
}


State::State(const State& other) : BoxEntity(other)
{
    custom_type = other.custom_type;
    name = other.name;
    type = other.type;

    initial_args = other.initial_args;

    code = other.code;

    has_return_value = other.has_return_value;
    return_value = other.return_value;

    launch_synchronous = other.launch_synchronous;
    launch_task_name = other.launch_task_name;
    launch_args = other.launch_args;
    launch_result_variable = other.launch_result_variable;

    join_pid_variable = other.join_pid_variable;
    join_result_variable = other.join_result_variable;
    
    custom_config = other.custom_config;

    // had to define this because we need the vectors of transition pointers
    // to start empty instead of referring to old ones
}


json State::to_json()
{
    json jdata = BoxEntity::to_json();
    jdata["name"] = name;
    jdata["type"] = (int)(type);
    jdata["initial_args"] = json::array();
    jdata["custom_config"] = json::array();
    // code ignored, gets saved to py file for sake of source control
    jdata["has_return_value"] = has_return_value;
    jdata["return_value"] = return_value.to_json();
    jdata["launch_synchronous"] = launch_synchronous;
    jdata["launch_task_name"] = launch_task_name.to_json();
    jdata["launch_args"] = json::array();
    jdata["launch_result_variable"] = launch_result_variable;
    jdata["join_pid_variable"] = join_pid_variable;
    jdata["join_result_variable"] = join_result_variable;

    for(int i=0; i < initial_args.size(); i++)
        jdata["initial_args"].push_back(initial_args[i].to_json());

    for(int i=0; i < launch_args.size(); i++)
        jdata["launch_args"].push_back(launch_args[i].to_json());
       
    for(auto it = custom_config.begin(); it != custom_config.end(); it++)
    {
        string name = it->first;
        auto json_data = it->second.to_json();
        jdata["custom_config"].push_back(json_data);
    }
    
    return jdata;
}


EntityType State::get_entity_type()
{
    return STATE;
}


void State::add_transition(Transition* transition, bool incoming)
{
    if(incoming)
        incoming_transitions.push_back(transition);
    else
        outgoing_transitions.push_back(transition);
}


void State::remove_transition(Transition* transition, bool incoming)
{
    auto vec = incoming ? &incoming_transitions : &outgoing_transitions;
    
    int i;
    for(i = 0; i < vec->size(); i++)
    {
        if((*vec)[i] == transition)
            break;
    }

    vec->erase(vec->begin() + i);
}


void State::update_transition_positions()
{
    for(int i = 0; i < incoming_transitions.size(); i++)
        incoming_transitions[i]->update_positions();

    for(int i = 0; i < outgoing_transitions.size(); i++)
        outgoing_transitions[i]->update_positions();
}


vector<Transition*> State::get_all_transitions()
{
    vector<Transition*> result;
    
    for(int i = 0; i < incoming_transitions.size(); i++)
        result.push_back(incoming_transitions[i]);
    
    for(int i = 0; i < outgoing_transitions.size(); i++)
        result.push_back(outgoing_transitions[i]);
    
    return result;
}


///// Custom State type configuration stuff

CustomStateConfig State::get_custom_config(string parameter_name)
{
    return custom_config[parameter_name];
}


void State::set_custom_config(string parameter_name, LVOV value)
{
    custom_config[parameter_name] = CustomStateConfig(parameter_name, value);
}


void State::make_default_config(vector<CustomConfigDef>& defs)
{
    clear_custom_config();
    
    for(auto it = defs.begin(); it < defs.end(); it++)
    {
        string name = it->name;
        LVOV value = LVOV(it->type);
        set_custom_config(name, value);
    }
}


void State::clear_custom_config()
{
    custom_config = map<string, CustomStateConfig>();
}


vector<CustomStateConfig> State::get_all_config()
{
    auto result = vector<CustomStateConfig>();
    
    for(auto it = custom_config.begin(); it != custom_config.end(); it++)
        result.push_back(it->second);
    
    return result;
}
