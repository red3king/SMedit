#pragma once

#include "state_type_ctrl.h"
#include "controllers/value_input_controller.h"



class ReturnStateController : public StateTypeController
{
    public:
        ReturnStateController(HistoryManager* history_manager);

        void attach(Gtk::Container* parent_container);

    protected:
        void set_state_impl(Machine* owning_machine, State* state);

    private:
        ValueInputController* value_input_ctrl;
        Gtk::Box vic_box;
        Gtk::Grid main_grid;
        Gtk::Label has_retval_label;
        Gtk::Switch has_retval_switch;
        
        bool is_emitting, is_setting;
        void on_has_retval_changed();
        void on_retval_changed(LVOV retval);
        void _enable_vic(bool enabled);
};
