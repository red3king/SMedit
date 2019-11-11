#pragma once

#include "state_type_ctrl.h"


class JoinStateController : public StateTypeController
{
    public:
        JoinStateController(HistoryManager* history_manager, Gtk::Entry *pid_var_entry, Gtk::Entry *result_variable_entry);

    protected:
        void set_state_impl(Machine* owning_machine, State* state);

    private:
        Gtk::Entry *pid_var_entry, *result_variable_entry;

        bool is_emitting, is_setting;
        void on_pidvar_changed();
        void on_resultvar_changed();
};

