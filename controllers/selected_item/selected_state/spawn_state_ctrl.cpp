#include "spawn_state_ctrl.h"
#include "const.h"
#include "utils.h"
#include "historymanager/operations/state_ops.h"


SpawnStateController::SpawnStateController(HistoryManager* history_manager) : StateTypeController(history_manager)
{
    is_emitting = false;
    is_setting = false;

    // prep widgets
    main_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    left_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    right_box.set_orientation(Gtk::ORIENTATION_VERTICAL);

    output_name_label.set_markup("<b>Output variable:</b>");
    set_margins(&output_name_label, 4);
    output_name_label.set_margin_top(14);
    left_align(&output_name_label);

    set_margins(&output_name_entry, 4);

    vector<ValueType> allowed_types = { VT_STR, VT_REF };
    task_name_input = new ValueInputController(allowed_types, "Task Name:");

    synchronous_label.set_markup("<b>Block</b>");
    set_margins(&synchronous_label, 4);
    set_margins(&synchronous_switch, 4);
    synchronous_label.set_margin_top(14);
    left_align(&synchronous_label);
    left_align(&synchronous_switch);


    argument_list_controller = new ArgumentListController();

    // attach widgets
    main_box.pack_start(left_box, false, false, 0);
    main_box.pack_start(right_box, false, false, 0);

    left_box.pack_start(value_input_box, false, false, 0);
    left_box.pack_start(output_name_label, false, false, 0);
    left_box.pack_start(output_name_entry, false, false, 0);
    left_box.pack_start(synchronous_label, false, false, 0);
    left_box.pack_start(synchronous_switch, false, false, 0);

    task_name_input->attach(&value_input_box);
    argument_list_controller->attach(&right_box);

    // signals
    output_name_entry.signal_changed().connect(sigc::mem_fun(this, &SpawnStateController::on_output_name_changed));
    task_name_input->value_changed_signal.connect(sigc::mem_fun(this, &SpawnStateController::on_task_name_changed));
    argument_list_controller->on_args_changed.connect(sigc::mem_fun(this, &SpawnStateController::on_args_changed));
    synchronous_switch.property_active().signal_changed().connect(sigc::mem_fun(this, &SpawnStateController::on_synch_changed));
}


void SpawnStateController::attach(Gtk::Container* parent_container)
{ 
    parent_container->add(main_box);
    output_name_label.show();
    output_name_entry.show();
    value_input_box.show();
    synchronous_label.show();
    synchronous_switch.show();
    left_box.show();
    right_box.show();
    main_box.show();
}


void SpawnStateController::on_synch_changed()
{
    if(is_setting)
        return;

    is_emitting = true;
    auto op = OpStateLaunchSynchronous(owning_machine, selected_state, synchronous_switch.get_state());
    history_manager->submit_operation(op);
    is_emitting = false;
}


void SpawnStateController::set_state_impl(Machine* owning_machine, State* state)
{
    if(is_emitting || state == nullptr)
        return;

    is_setting = true;
    output_name_entry.set_text(state->launch_result_variable);
    task_name_input->set_value(state->launch_task_name);
    argument_list_controller->set_args(state->launch_args);
    synchronous_switch.set_state(state->launch_synchronous);
    is_setting = false;
}


void SpawnStateController::on_output_name_changed()
{
    if(is_setting)
        return;

    is_emitting = true;
    auto varname = output_name_entry.get_text();
    auto op = OpStateLaunchResultVariable(owning_machine, selected_state, varname);
    history_manager->submit_operation(op);
    is_emitting = false;
}


void SpawnStateController::on_task_name_changed(LVOV task_name_value)
{
    if(is_setting)
        return;

    is_emitting = true;
    auto op = OpStateLaunchTaskName(owning_machine, selected_state, task_name_value);
    history_manager->submit_operation(op);
    is_emitting = false;
}


void SpawnStateController::on_args_changed(vector<Arg> task_args)
{
    if(is_setting)
        return;

    is_emitting = true;
    auto op = OpStateLaunchArgs(owning_machine, selected_state, task_args);
    history_manager->submit_operation(op);
    is_emitting = false;
}

