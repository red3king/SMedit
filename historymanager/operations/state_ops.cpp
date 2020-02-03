#include "state_ops.h"
#include "transition_ops.h"  // for delete_transition()
#include "signals.h"



// Abstract modify
StateChgOperation::StateChgOperation(Machine *machine, State *state)
{
    machine_id = machine->id;
    state_id = state->id;
}


unsigned int StateChgOperation::execute(Project& project)
{
    Machine *machine = project.get_machine_by_id(machine_id);
    State *state = machine->get_state_by_id(state_id);
    execute_impl(state);
    signals.fire_model_changed(STATE, MODIFY, state_id);
    return state_id;
}


// Create

OpStateCreate::OpStateCreate(Machine *machine, float x, float y)
{
    machine_id = machine->id;
    this->x = x;
    this->y = y;
}


unsigned int OpStateCreate::execute(Project& project)
{
    State *state = new State(project.get_next_id());
    state->name = "new state";
    state->type = CODE;
    state->x = x;
    state->y = y;
    state->w = 150;
    state->h = 125;

    Machine *machine = project.get_machine_by_id(machine_id);
    machine->states.push_back(state);

    signals.fire_model_changed(STATE, CREATE, state->id);
    return state->id;
}


OpStateCreate *OpStateCreate::clone()
{
    return new OpStateCreate(*this);
}


// Delete

OpStateDelete::OpStateDelete(Machine *machine, State *state)
{
    machine_id = machine->id;
    state_id = state->id;
}


void delete_state(Machine *machine, State *state)
{
    signals.fire_model_changed(STATE, PRE_DELETE, state->id);
   
    // Unlink attached incoming transitions
    for(int i=0; i<state->incoming_transitions.size(); i++)
    {
        Transition *transition = state->incoming_transitions[i];
        transition->to_state = nullptr;
        transition->update_positions();
    }

    vector<Transition*> to_delete;
    
    for(int i=0; i<state->outgoing_transitions.size(); i++)
    {
        Transition *transition = state->outgoing_transitions[i];
        
        // custom states and their outgoing transitions are created and deleted with each other
        if(state->is_custom())
            //delete_transition(machine, transition);
            to_delete.push_back(transition);
            
        // normal states leave their transitions alone when deleted:
        else
        {
            transition->from_state = nullptr;
            transition->update_positions();
        }
    }
    
    for(int i=0; i<to_delete.size(); i++)
        delete_transition(machine, to_delete[i]);

    int sindex = -1;
    while(machine->states[++sindex] != state);
    
    delete state;
    machine->states.erase(machine->states.begin() + sindex);  
}


unsigned int OpStateDelete::execute(Project& project)
{
    Machine *machine = project.get_machine_by_id(machine_id);
    State *to_delete;
    unsigned int deleted_id;
    int i;

    for(i=0; i<machine->states.size(); i++)
    {
        if(machine->states[i]->id == state_id)
        {
            to_delete = machine->states[i];
            break;
        }
    }
    
    deleted_id = to_delete->id;
    delete_state(machine, to_delete);
    return deleted_id;
}


OpStateDelete *OpStateDelete::clone()
{
    return new OpStateDelete(*this);
}


// Move

OpStateMove::OpStateMove(Machine *machine, State *state, float x, float y)
{
    machine_id = machine->id;
    state_id = state->id;
    this->x = x;
    this->y = y;
}


unsigned int OpStateMove::execute(Project& project)
{
    Machine *machine = project.get_machine_by_id(machine_id);
    State *state = machine->get_state_by_id(state_id);
    state->x = x;
    state->y = y;
    state->update_transition_positions();
    return state->id;
}


bool OpStateMove::may_collapse_impl(Operation& other)
{
    OpStateMove& other_move = dynamic_cast<OpStateMove&>(other);
    return other_move.state_id == state_id;
}


void OpStateMove::collapse(Operation& other)
{
    OpStateMove& other_move = dynamic_cast<OpStateMove&>(other);
    x = other_move.x;
    y = other_move.y;
}


OpStateMove *OpStateMove::clone()
{
    return new OpStateMove(*this);
}



// type 

OpStateType::OpStateType(Machine* machine, State* state, int type) : StateChgOperation(machine, state)
{
    this->type = type;
}


OpStateType* OpStateType::clone()
{
    return new OpStateType(*this);
}



unsigned int OpStateType::execute(Project& project)
{
    Machine *machine = project.get_machine_by_id(machine_id);
    State *state = machine->get_state_by_id(state_id);
    
    if(state_type_is_custom(state->type))
    {
        vector<Transition*> to_delete = state->outgoing_transitions;
        // Delete all outgoing transitions
        for(int i=0; i<to_delete.size(); i++)
        {
            Transition *transition = to_delete[i];
            delete_transition(machine, transition);
        }
    }
    
    state->type = type;

    if(state_type_is_custom(type))
    {
        // Create outgoing transitions
        CustomStateClass *cs_class = project.get_custom_state_class_by_id(type);
        state->custom_type = cs_class;

        for(int i=0; i<cs_class->transition_defs.size(); i++)
        {
            auto transition_def = cs_class->transition_defs[i];
            float to_x = state->x - 250;
            float to_y = state->y + 50 - (50 * i);
            Transition *t = create_transition(project, machine_id, true, 0, 0, to_x, to_y, 0);
            t->from_state = state;
            t->type = transition_def.type;
            t->timeout = transition_def.timeout;
            t->event_name = transition_def.event_name;
            state->add_transition(t, false);
            t->update_positions();
        }
    }

    else
        state->custom_type = nullptr;
    
    signals.fire_model_changed(STATE, MODIFY, state_id);
    return state_id;
}


void OpStateType::execute_impl(State* state) { }


bool OpStateType::may_collapse_impl(Operation& other)
{
    auto op = (OpStateType&) other;
    return state_id == op.state_id;
}


void OpStateType::collapse(Operation& other)
{
    auto op = (OpStateType&) other;
    type = op.type;
}





// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE

/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.op_specs import *

for line in state_opgen.make_cpp():
    cog.outl(line)

]]]*/


// name  -  AUTOGENERATED, DO NOT MODIFY

OpStateName::OpStateName(Machine* machine, State* state, string name) : StateChgOperation(machine, state)
{
    this->name = name;
}


OpStateName* OpStateName::clone()
{
    return new OpStateName(*this);
}


void OpStateName::execute_impl(State* state)
{
    state->name = name;
}


bool OpStateName::may_collapse_impl(Operation& other)
{
    auto op = (OpStateName&) other;
    return state_id == op.state_id;
}


void OpStateName::collapse(Operation& other)
{
    auto op = (OpStateName&) other;
    name = op.name;
}


// code  -  AUTOGENERATED, DO NOT MODIFY

OpStateCode::OpStateCode(Machine* machine, State* state, string code) : StateChgOperation(machine, state)
{
    this->code = code;
}


OpStateCode* OpStateCode::clone()
{
    return new OpStateCode(*this);
}


void OpStateCode::execute_impl(State* state)
{
    state->code = code;
}


bool OpStateCode::may_collapse_impl(Operation& other)
{
    auto op = (OpStateCode&) other;
    return state_id == op.state_id;
}


void OpStateCode::collapse(Operation& other)
{
    auto op = (OpStateCode&) other;
    code = op.code;
}


// join_pid_variable  -  AUTOGENERATED, DO NOT MODIFY

OpStateJoinPidVariable::OpStateJoinPidVariable(Machine* machine, State* state, string join_pid_variable) : StateChgOperation(machine, state)
{
    this->join_pid_variable = join_pid_variable;
}


OpStateJoinPidVariable* OpStateJoinPidVariable::clone()
{
    return new OpStateJoinPidVariable(*this);
}


void OpStateJoinPidVariable::execute_impl(State* state)
{
    state->join_pid_variable = join_pid_variable;
}


bool OpStateJoinPidVariable::may_collapse_impl(Operation& other)
{
    auto op = (OpStateJoinPidVariable&) other;
    return state_id == op.state_id;
}


void OpStateJoinPidVariable::collapse(Operation& other)
{
    auto op = (OpStateJoinPidVariable&) other;
    join_pid_variable = op.join_pid_variable;
}


// join_result_variable  -  AUTOGENERATED, DO NOT MODIFY

OpStateJoinResultVariable::OpStateJoinResultVariable(Machine* machine, State* state, string join_result_variable) : StateChgOperation(machine, state)
{
    this->join_result_variable = join_result_variable;
}


OpStateJoinResultVariable* OpStateJoinResultVariable::clone()
{
    return new OpStateJoinResultVariable(*this);
}


void OpStateJoinResultVariable::execute_impl(State* state)
{
    state->join_result_variable = join_result_variable;
}


bool OpStateJoinResultVariable::may_collapse_impl(Operation& other)
{
    auto op = (OpStateJoinResultVariable&) other;
    return state_id == op.state_id;
}


void OpStateJoinResultVariable::collapse(Operation& other)
{
    auto op = (OpStateJoinResultVariable&) other;
    join_result_variable = op.join_result_variable;
}


// return_value  -  AUTOGENERATED, DO NOT MODIFY

OpStateReturnValue::OpStateReturnValue(Machine* machine, State* state, LVOV return_value) : StateChgOperation(machine, state)
{
    this->return_value = return_value;
}


OpStateReturnValue* OpStateReturnValue::clone()
{
    return new OpStateReturnValue(*this);
}


void OpStateReturnValue::execute_impl(State* state)
{
    state->return_value = return_value;
}


bool OpStateReturnValue::may_collapse_impl(Operation& other)
{
    auto op = (OpStateReturnValue&) other;
    return state_id == op.state_id;
}


void OpStateReturnValue::collapse(Operation& other)
{
    auto op = (OpStateReturnValue&) other;
    return_value = op.return_value;
}


// initial_args  -  AUTOGENERATED, DO NOT MODIFY

OpStateInitialArgs::OpStateInitialArgs(Machine* machine, State* state, vector<ArgDef> initial_args) : StateChgOperation(machine, state)
{
    this->initial_args = initial_args;
}


OpStateInitialArgs* OpStateInitialArgs::clone()
{
    return new OpStateInitialArgs(*this);
}


void OpStateInitialArgs::execute_impl(State* state)
{
    state->initial_args = initial_args;
}


bool OpStateInitialArgs::may_collapse_impl(Operation& other)
{
    auto op = (OpStateInitialArgs&) other;
    return state_id == op.state_id;
}


void OpStateInitialArgs::collapse(Operation& other)
{
    auto op = (OpStateInitialArgs&) other;
    initial_args = op.initial_args;
}


// launch_task_name  -  AUTOGENERATED, DO NOT MODIFY

OpStateLaunchTaskName::OpStateLaunchTaskName(Machine* machine, State* state, LVOV launch_task_name) : StateChgOperation(machine, state)
{
    this->launch_task_name = launch_task_name;
}


OpStateLaunchTaskName* OpStateLaunchTaskName::clone()
{
    return new OpStateLaunchTaskName(*this);
}


void OpStateLaunchTaskName::execute_impl(State* state)
{
    state->launch_task_name = launch_task_name;
}


bool OpStateLaunchTaskName::may_collapse_impl(Operation& other)
{
    auto op = (OpStateLaunchTaskName&) other;
    return state_id == op.state_id;
}


void OpStateLaunchTaskName::collapse(Operation& other)
{
    auto op = (OpStateLaunchTaskName&) other;
    launch_task_name = op.launch_task_name;
}


// launch_args  -  AUTOGENERATED, DO NOT MODIFY

OpStateLaunchArgs::OpStateLaunchArgs(Machine* machine, State* state, vector<Arg> launch_args) : StateChgOperation(machine, state)
{
    this->launch_args = launch_args;
}


OpStateLaunchArgs* OpStateLaunchArgs::clone()
{
    return new OpStateLaunchArgs(*this);
}


void OpStateLaunchArgs::execute_impl(State* state)
{
    state->launch_args = launch_args;
}


bool OpStateLaunchArgs::may_collapse_impl(Operation& other)
{
    auto op = (OpStateLaunchArgs&) other;
    return state_id == op.state_id;
}


void OpStateLaunchArgs::collapse(Operation& other)
{
    auto op = (OpStateLaunchArgs&) other;
    launch_args = op.launch_args;
}


// launch_result_variable  -  AUTOGENERATED, DO NOT MODIFY

OpStateLaunchResultVariable::OpStateLaunchResultVariable(Machine* machine, State* state, string launch_result_variable) : StateChgOperation(machine, state)
{
    this->launch_result_variable = launch_result_variable;
}


OpStateLaunchResultVariable* OpStateLaunchResultVariable::clone()
{
    return new OpStateLaunchResultVariable(*this);
}


void OpStateLaunchResultVariable::execute_impl(State* state)
{
    state->launch_result_variable = launch_result_variable;
}


bool OpStateLaunchResultVariable::may_collapse_impl(Operation& other)
{
    auto op = (OpStateLaunchResultVariable&) other;
    return state_id == op.state_id;
}


void OpStateLaunchResultVariable::collapse(Operation& other)
{
    auto op = (OpStateLaunchResultVariable&) other;
    launch_result_variable = op.launch_result_variable;
}


// launch_synchronous  -  AUTOGENERATED, DO NOT MODIFY

OpStateLaunchSynchronous::OpStateLaunchSynchronous(Machine* machine, State* state, bool launch_synchronous) : StateChgOperation(machine, state)
{
    this->launch_synchronous = launch_synchronous;
}


OpStateLaunchSynchronous* OpStateLaunchSynchronous::clone()
{
    return new OpStateLaunchSynchronous(*this);
}


void OpStateLaunchSynchronous::execute_impl(State* state)
{
    state->launch_synchronous = launch_synchronous;
}


bool OpStateLaunchSynchronous::may_collapse_impl(Operation& other)
{
    auto op = (OpStateLaunchSynchronous&) other;
    return state_id == op.state_id;
}


void OpStateLaunchSynchronous::collapse(Operation& other)
{
    auto op = (OpStateLaunchSynchronous&) other;
    launch_synchronous = op.launch_synchronous;
}


// has_return_value  -  AUTOGENERATED, DO NOT MODIFY

OpStateHasReturnValue::OpStateHasReturnValue(Machine* machine, State* state, bool has_return_value) : StateChgOperation(machine, state)
{
    this->has_return_value = has_return_value;
}


OpStateHasReturnValue* OpStateHasReturnValue::clone()
{
    return new OpStateHasReturnValue(*this);
}


void OpStateHasReturnValue::execute_impl(State* state)
{
    state->has_return_value = has_return_value;
}


bool OpStateHasReturnValue::may_collapse_impl(Operation& other)
{
    auto op = (OpStateHasReturnValue&) other;
    return state_id == op.state_id;
}


void OpStateHasReturnValue::collapse(Operation& other)
{
    auto op = (OpStateHasReturnValue&) other;
    has_return_value = op.has_return_value;
}


//[[[end]]]

