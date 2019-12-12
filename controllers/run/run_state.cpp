#include "run_state.h"


// RunningMachine

RunningMachine::RunningMachine(int id, int machine_def_id)
{
    this->id = id;
    this->machine_def_id = machine_def_id;
    current_state_def_id = -1;
}




// RunningState

RunningState::RunningState(BroadcastEvents& broadcast_events)
{
    current_project = nullptr;
    current_machine_id = -1;

    broadcast_events.machine_created.connect(sigc::mem_fun(*this, &RunningState::on_machine_created));
    broadcast_events.machine_deleted.connect(sigc::mem_fun(*this, &RunningState::on_machine_deleted));
    broadcast_events.machine_state_changed.connect(sigc::mem_fun(*this, &RunningState::on_machine_state_changed));
}


void RunningState::set_project(Project* current_project)
{
    this->current_project = current_project;
}


void RunningState::on_machine_created(int machine_id, int machine_def_id)
{
    running_machines.push_back(RunningMachine(machine_id, machine_def_id));    
    current_machine_id = machine_id;

    auto machine_def = current_project->get_machine_by_id(machine_def_id);
    select_machine.emit(machine_def);
}


void RunningState::on_machine_deleted(int machine_id)
{
    // update state
    int i=0;
    for(; i<running_machines.size(); i++)
    {
        if(running_machines[i].id == machine_id)
            break;
    }

    running_machines.erase(running_machines.begin() + i);

    // emit signals if current deleted
    if(machine_id == current_machine_id)
    {
        if(running_machines.size() == 0)
        {
            current_machine_id = -1;
            select_machine.emit(nullptr);
            return;
        }

        // TODO actually keep track of the machines spawn stack
        // so we can go back to the spawner of the machine that just died.
        // for this we will have to modify the machine created broadcast to include id
        // of the parent machine
        auto running_machine = running_machines[running_machines.size() - 1];
        auto machine_def = current_project->get_machine_by_id(running_machine.machine_def_id);
        select_machine.emit(machine_def);
        select_state.emit(running_machine.current_state_def_id);
    }
}


void RunningState::on_machine_state_changed(int machine_id, int state_def_id, json state_vars)
{
    for(int i=0; i<running_machines.size(); i++)
    {
        if(running_machines[i].id == machine_id)
        {
            running_machines[i].current_state_def_id = state_def_id;
            running_machines[i].state_vars = state_vars;
            break;
        }
    }

    if(machine_id == current_machine_id)
        select_state.emit(state_def_id);
}


