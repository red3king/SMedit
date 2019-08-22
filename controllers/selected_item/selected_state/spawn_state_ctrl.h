#pragma once

#include "state_type_ctrl.h"
#include "controllers/argument_list_controller.h"

  
class SpawnStateController : public StateTypeController
{
    public:
        SpawnStateController(HistoryManager* history_manager);

        void attach(Gtk::Container* parent_container);

    protected:
        void set_state_impl(Machine* owning_machine, State* state);

    private:
        Gtk::Box main_box, left_box, right_box;
        Gtk::Box value_input_box;
        ValueInputController* task_name_input;
        Gtk::Label output_name_label;
        Gtk::Entry output_name_entry;
        Gtk::Label synchronous_label;
        Gtk::Switch synchronous_switch;
        
        ArgumentListController* argument_list_controller;

        bool is_emitting, is_setting;

        void on_output_name_changed();
        void on_synch_changed();
        void on_task_name_changed(LVOV task_name_value);
        void on_args_changed(vector<Arg> task_args);
};
