#include "return_state_ctrl.h"
#include "historymanager/operations/state_ops.h"


ReturnStateController::ReturnStateController(HistoryManager* history_manager) : StateTypeController(history_manager)
{
    is_emitting = false;
    is_setting = false;

    value_input_ctrl = new ValueInputController();
    value_input_ctrl->attach(&vic_box);
    value_input_ctrl->value_changed_signal.connect(sigc::mem_fun(this, &ReturnStateController::on_retval_changed));
    
    // prep widgets
    has_retval_label.set_markup("<b>Has return value</b>");
    left_align(&has_retval_label);
    set_margins(&has_retval_label, 4);

    has_retval_switch.property_active().signal_changed().connect(sigc::mem_fun(this, &ReturnStateController::on_has_retval_changed));
    right_align(&has_retval_switch);
    set_margins(&has_retval_switch, 4);

    main_grid.attach(has_retval_label, 0, 0, 1, 1);
    main_grid.attach(has_retval_switch, 1, 0, 1, 1);
    main_grid.attach(vic_box, 0, 1, 2, 2);    
}


void ReturnStateController::attach(Gtk::Container* parent_container)
{
    parent_container->add(main_grid);

    main_grid.show();
    vic_box.show();
    has_retval_label.show();
    has_retval_switch.show();
}


void ReturnStateController::set_state_impl(Machine* owning_machine, State* state)
{
    has_retval_switch.set_state(state->has_return_value);
    value_input_ctrl->set_value(state->return_value);
    _enable_vic(state->has_return_value);
}


void ReturnStateController::on_has_retval_changed()
{
    bool has_retval = has_retval_switch.get_state();
    _enable_vic(has_retval);

    if(is_setting)
        return;

    is_emitting = true;
    auto op = OpStateHasReturnValue(owning_machine, selected_state, has_retval);
    history_manager->submit_operation(op);
    is_emitting = false;
}


void ReturnStateController::on_retval_changed(LVOV retval)
{
    if(is_setting)
        return;

    is_emitting = true;
    auto op = OpStateReturnValue(owning_machine, selected_state, retval);
    history_manager->submit_operation(op);
    is_emitting = false;
}


void ReturnStateController::_enable_vic(bool enabled)
{
    if(!enabled)
        value_input_ctrl->set_value(LVOV(""));

    value_input_ctrl->set_enabled(enabled);
}

