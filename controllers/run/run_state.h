#pragma once

#include <vector>
#include <sigc++/sigc++.h>
#include "lib/json.hpp"

#include "net/broadcast_events.h"
#include "models/project.h"

using std::vector;


class RunningMachine
{
    public:
        RunningMachine(int id, int machine_def_id, string name);

        int id;
        string name;
        int machine_def_id;
        int current_state_def_id;
        json state_vars;
};


class RunningState
{
    // Keeps track of the current state each running machine is in on the server
    // Also emits signals to update the GUI area

    public:
        RunningState(BroadcastEvents& broadcast_events);
        
        void set_project(Project* current_project);

        void user_select_machine(int machine_id);   // user clicked on a machine, emit the signal. 

        vector<RunningMachine>& get_running_machines();

        // signals for the GUIContext & GUIState
        sigc::signal<void, Machine*> select_machine;    // may be nullptr to un-select
        sigc::signal<void, int> select_state;

    private:
        Project* current_project;
        vector<RunningMachine> running_machines;
        int current_machine_id;  // id of RunningMachine displayed on screen. not def id.

        void on_machine_created(int machine_id, int machine_def_id);
        void on_machine_deleted(int machine_id);
        void on_machine_state_changed(int machine_id, int state_def_id, json state_vars);
};
