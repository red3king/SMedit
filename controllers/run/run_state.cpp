#include "run_state.h"
#include "log.h"


// RunningMachine

RunningMachine::RunningMachine(int id, int machine_def_id, string name)
{
    this->id = id;
    this->machine_def_id = machine_def_id;
    this->name = name;
    current_state_def_id = -1;
    terminated = false;
    created = false;
}



// RunningState

RunningState::RunningState(BroadcastEvents& broadcast_events)
{
    current_project = nullptr;
    current_machine_id = -1;
    terminated_timer_running = false;

    broadcast_events.machine_created.connect(sigc::mem_fun(*this, &RunningState::on_machine_created));
    broadcast_events.machine_deleted.connect(sigc::mem_fun(*this, &RunningState::on_machine_deleted));
    broadcast_events.machine_state_changed.connect(sigc::mem_fun(*this, &RunningState::on_machine_state_changed));
}


void RunningState::set_project(Project* current_project)
{
    this->current_project = current_project;
}


RunningMachine& RunningState::get_running_machine(int machine_id)
{
    for(int i=0; i<running_machines.size(); i++)
    {
        if(running_machines[i].id == machine_id)
            return running_machines.at(i);
    }

    return running_machines.at(0);
}


void RunningState::user_select_machine(int machine_id)
{
    for(int i=0; i<running_machines.size(); i++)
    {
        auto rm = running_machines[i];
        
        if(rm.id == machine_id)
        {
            auto machine_def = current_project->get_machine_by_id(rm.machine_def_id);
            fire_machine_selected(rm.id, machine_def);
            return;
        }
    } 

    fire_machine_selected(-1, nullptr);
}


vector<RunningMachine>& RunningState::get_running_machines()
{
    return running_machines;
}


void RunningState::initial_state_synch(json machines_list)
{
    running_machines.clear();
    current_machine_id = -1;

    for(int i=0; i<machines_list.size(); i++)
    {
        json machine = machines_list[i];
        int id = machine["id"];
        int machine_def_id = machine["machine_def_id"];
        int current_state_def_id = machine["current_state_def_id"];
        auto rm = make_running_machine(id, machine_def_id, current_state_def_id);
        running_machines.push_back(rm);
    }

    int l = running_machines.size();

    if(l > 0)
    {
        auto rm = running_machines[l-1];
        auto machine_def = current_project->get_machine_by_id(rm.machine_def_id);
        fire_machine_selected(rm.id, machine_def);
        select_state.emit(rm.current_state_def_id);
    }
}


RunningMachine RunningState::make_running_machine(int id, int machine_def_id, int state_def_id)
{
    auto machine_def = current_project->get_machine_by_id(machine_def_id);
    string name = machine_def->name + "__" + std::to_string(id);
    auto machine = RunningMachine(id, machine_def_id, name);
    machine.current_state_def_id = state_def_id;
    return machine;
}


void RunningState::on_machine_created(int machine_id, int machine_def_id)
{
    // For the first machine created, fire the signal immediately
    // for subsequent machines, use the timer to delay the signal.

    bool has_current_machine = current_machine_id != -1;

    if(has_current_machine)
        _finish_timer_if_active();
    
    auto rm = make_running_machine(machine_id, machine_def_id);
    rm.created = has_current_machine;
    running_machines.push_back(rm);       

    if(has_current_machine)
        _start_timer();

    else
    {
        Machine* machine_def = current_project->get_machine_by_id(machine_def_id);
        fire_machine_selected(rm.id, machine_def);
    }
}


void RunningState::_finish_timer_if_active()
{
    if(terminated_timer_running)
    {
        // cancel terminated timer
        terminated_timer_connection.disconnect();

        // immediately act as if current timer finished
        on_terminated_timer_tick();
    }
}


void RunningState::_start_timer()
{
    terminated_timer_running = true;
    terminated_timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &RunningState::on_terminated_timer_tick), VIEW_DELAY_MS);   
}


void RunningState::on_machine_deleted(int machine_id)
{
    _finish_timer_if_active();
    
    for(int i=0; i<running_machines.size(); i++)
    {
        if(running_machines[i].id == machine_id)
            running_machines[i].terminated = true;
    }

    _start_timer();
}


bool RunningState::on_terminated_timer_tick()
{
    terminated_timer_running = false;

    // find the terminated machine
    int i=0;
    int machine_id = 0;
    bool created;
    for(; i<running_machines.size(); i++)
    {
        if(running_machines[i].terminated || running_machines[i].created)
        {
            machine_id = running_machines[i].id;
            created = running_machines[i].created;
            break;
        }
    }

    if(created)
    {
        // emit signals when new machine created
        auto rm = running_machines.at(i);
        auto machine_def = current_project->get_machine_by_id(rm.machine_def_id);
        running_machines[i].created = false;
        fire_machine_selected(rm.id, machine_def); 
        select_state.emit(rm.current_state_def_id);
    }
    
    else
    {
        running_machines.erase(running_machines.begin() + i);

        // emit signals only if current selected machine deleted
        if(machine_id == current_machine_id)
        {
            if(running_machines.size() == 0)
            {
                fire_machine_selected(-1, nullptr);
                return false;
            }

            // TODO actually keep track of the machines spawn stack
            // so we can go back to the spawner of the machine that just died.
            // for this we will have to modify the machine created broadcast to include id
            // of the parent machine
            auto running_machine = running_machines[running_machines.size() - 1];
            auto machine_def = current_project->get_machine_by_id(running_machine.machine_def_id);

            fire_machine_selected(running_machine.id, machine_def);
            select_state.emit(running_machine.current_state_def_id);
        }
    }

    return false;
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


void RunningState::fire_machine_selected(int id, Machine* machine)
{
    if(current_machine_id != -1)
       select_machine.emit(-1, nullptr);

    select_machine.emit(id, machine);
    current_machine_id = id;
}

