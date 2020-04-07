#include "custom_state_ctrl.h"
#include "historymanager/operations/state_ops.h"


CustomStateController::CustomStateController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : StateTypeController(history_manager), config_entry_ctrl(builder)
{
    builder->get_widget("custom_state_name_label", name_label);
    builder->get_widget("custom_state_detail_label", detail_label);
    
    config_entry_ctrl.config_changed.connect(sigc::mem_fun(this, &CustomStateController::on_config_changed));
}



void CustomStateController::set_state_impl(Machine* owning_machine, State* selected_state)
{
    _update();
}


void CustomStateController::on_config_changed(string parameter_name)
{
    CustomStateConfig cfg = config_entry_ctrl.get_config(parameter_name);
    auto op = OpStateConfig(owning_machine, selected_state, cfg);
    history_manager->submit_operation(op);
}


void CustomStateController::_update()
{
    bool hs = has_state();
    
    if(!hs || !selected_state->is_custom())
    {
        name_label->set_text("");
        detail_label->set_text("");
        return;
    }
    
    CustomStateClass* custom_type = selected_state->custom_type;
    name_label->set_text(custom_type->name);
    detail_label->set_text(custom_type->has_config() ? "settings:" : "nothing to configure");

    auto config_list = selected_state->get_all_config();
    config_entry_ctrl.set_config(config_list);
}
