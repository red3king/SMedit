#pragma once

#include <gtkmm.h>
#include "net/broadcast_events.h"
#include "controllers/run/run_state.h"
#include "controllers/list_view_controller.h"



class MachinesListController
{
    /*
        display list of current machines
            -> add get machines list function to runningstate

        should auto select row corresponding to new machine (when the auto select setting = true)
            -> using machine added event

        should let the user select a machine when they click on it
            -> call the force user select function on click.

    */


    public:
        MachinesListController(Glib::RefPtr<Gtk::Builder> const& builder, BroadcastEvents& broadcast_events, RunningState* running_state);

    private:
        bool selecting;

        ListViewController* list_view_controller;
        RunningState* running_state;

        void on_machine_select(int machine_id, Project* project, Machine* machine_def);
        void on_selection_changed(unsigned int machine_id, bool from_user);

        void _update_list(int machine_id = -1);
};


