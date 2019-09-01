#include "state.h"
#include "transition.h"


string state_type_to_string(StateType type)
{
    switch(type)
    {
        case INITIAL:
            return STS_INITIAL;
        case CODE:
            return STS_CODE;
        case RETURN:
            return STS_RETURN;
        case SPAWN:
            return STS_SPAWN;
        case JOIN:
            return STS_JOIN;
    }

    throw std::invalid_argument("unknown StateType");
}


StateType string_to_state_type(string input)
{
    if(input == STS_INITIAL)
        return INITIAL;
    if(input == STS_CODE)
        return CODE;
    if(input == STS_RETURN)
        return RETURN;
    if(input == STS_SPAWN)
        return SPAWN;
    if(input == STS_JOIN)
        return JOIN;

    throw std::invalid_argument("unknown StateType string");
}



State::State(unsigned int id) : BoxEntity(id) 
{ 
    string empty = "";
    launch_task_name = LVOV(empty);
    has_return_value = false;
} 


State::State(json jdata) : BoxEntity(jdata), return_value(jdata["return_value"]), launch_task_name(jdata["launch_task_name"])
{
    name = jdata["name"];
    type = (StateType) jdata["type"];
    
    for(int i=0; i<jdata["initial_args"].size(); i++)
        initial_args.push_back(ArgDef(jdata["initial_args"][i]));

    has_return_value = jdata["has_return_value"];
    launch_synchronous = jdata["launch_synchronous"];
    
    for(int i=0; i<jdata["launch_args"].size(); i++)
        launch_args.push_back(Arg(jdata["launch_args"][i]));

    launch_result_variable = jdata["launch_result_variable"];
    join_pid_variable = jdata["join_pid_variable"];    
}


json State::to_json()
{
    json jdata = BoxEntity::to_json();
    jdata["name"] = name;
    jdata["type"] = (int)(type);
    jdata["initial_args"] = json::array();
    // code ignored, gets saved to file
    jdata["has_return_value"] = has_return_value;
    jdata["return_value"] = return_value.to_json();
    jdata["launch_synchronous"] = launch_synchronous;
    jdata["launch_task_name"] = launch_task_name.to_json();
    jdata["launch_args"] = json::array();
    jdata["launch_result_variable"] = launch_result_variable;
    jdata["join_pid_variable"] = join_pid_variable;
    
    for(int i=0; i<initial_args.size(); i++)
        jdata["initial_args"].push_back(initial_args[i].to_json());

    for(int i=0; i<launch_args.size(); i++)
        jdata["launch_args"].push_back(launch_args[i].to_json());
    
    return jdata;
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
    for(i=0; i<vec->size(); i++)
    {
        if((*vec)[i] == transition)
            break;
    }

    vec->erase(vec->begin() + i);
}


void State::update_transition_positions()
{
    for(int i=0; i<incoming_transitions.size(); i++)
        incoming_transitions[i]->update_positions();

    for(int i=0; i<outgoing_transitions.size(); i++)
        outgoing_transitions[i]->update_positions();
}
