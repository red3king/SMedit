#include "join_state_ctrl.h"
#include "historymanager/operations/state_ops.h"


JoinStateController::JoinStateController(HistoryManager *history_manager, Gtk::Entry *pid_var_entry, Gtk::Entry *result_variable_entry) : StateTypeController(history_manager)
{
    is_emitting = false;
    is_setting = false;

    this->pid_var_entry = pid_var_entry;
    this->result_variable_entry = result_variable_entry;

    pid_var_entry->signal_changed().connect(sigc::mem_fun(this, &JoinStateController::on_pidvar_changed));
    result_variable_entry->signal_changed().connect(sigc::mem_fun(this, &JoinStateController::on_resultvar_changed));
}


void JoinStateController::set_state_impl(Machine *owning_machine, State *state)
{
    if(is_emitting || state == nullptr)
        return;  // TODO - I have a couple state tyep controllers, but they aren't all implemented like this
                // need to take this if statement and put it in the non-"impl" function
                // and make all state type controllers derive from StateTypeController for consistency,
                // and define is_setting and is_emitting in the StateTypeController (and teset if they're really 
                // needed)

    is_setting = true;
    pid_var_entry->set_text(state->join_pid_variable);
    result_variable_entry->set_text(state->join_result_variable);
    is_setting = false;
}


void JoinStateController::on_pidvar_changed()
{
    if(is_setting)
        return;

    string pid_variable = pid_var_entry->get_text();
    auto op = OpStateJoinPidVariable(owning_machine, selected_state, pid_variable);
    is_emitting = true;
    history_manager->submit_operation(op);
    is_emitting = false;
}


void JoinStateController::on_resultvar_changed()
{
    if(is_setting)
        return;

    string result_variable = result_variable_entry->get_text();
    auto op = OpStateJoinResultVariable(owning_machine, selected_state, result_variable);
    is_emitting = true;
    history_manager->submit_operation(op);
    is_emitting = false;
}
