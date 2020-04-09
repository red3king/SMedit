#pragma once

#include <vector>
#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include "lib/json.hpp"

#include "net/broadcast_events.h"
#include "models/project.h"

#define VIEW_DELAY_MS 900

using std::vector;
using nlohmann::json;


class RunningMachine
{
    public:
        RunningMachine(int id, int machine_def_id, string name);

        int id;
        string name;
        int machine_def_id;
        int current_state_def_id;
        json state_vars;
        bool terminated;  // runningmachines are kept around for a few seconds before being removed
        bool created;
};


class RunningState
{
    // Keeps track of the current state each running machine is in on the server
    // Also emits signals to update the GUI area

    // When a machine finishes ("terminated"), RunningState
    // waits for a few seconds before emitting the select_machine signal,
    // to give the UI time to show that machine enter its final state.

    public:
        RunningState(BroadcastEvents& broadcast_events);
        
        void set_project(Project* current_project);

        void user_select_machine(int machine_id);   // user clicked on a machine, emit the signal. 

        vector<RunningMachine>& get_running_machines();
        RunningMachine& get_running_machine(int machine_id);

        void initial_state_synch(json machines_list);

        // signals for the GUIContext & GUIState
        // these signals are delayed compared to the ones from broadcaststatus,
        // giving the user time to see things instead of immediately cutting the camera away.
        sigc::signal<void, int, Project*, Machine*> select_machine;    // int is running maching id. mach def may be nullptr to un-select
        sigc::signal<void, int> select_state;

    private:
        Project* current_project;
        vector<RunningMachine> running_machines;
        int current_machine_id;  // id of RunningMachine displayed on screen. not def id.

        bool terminated_timer_running;
        sigc::connection terminated_timer_connection;

        bool on_terminated_timer_tick();

        RunningMachine make_running_machine(int id, int machine_def_id, int state_def_id = -1);

        void _finish_timer_if_active();
        void _start_timer();

        void on_machine_created(int machine_id, int machine_def_id);
        void on_machine_deleted(int machine_id);
        void on_machine_state_changed(int machine_id, int state_def_id, json state_vars);

        void fire_machine_selected(int id, Machine* machine);
};
