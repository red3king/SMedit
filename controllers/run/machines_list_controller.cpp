#include "machines_list_controller.h"


MachinesListController::MachinesListController(Glib::RefPtr<Gtk::Builder> const& builder, BroadcastEvents& broadcast_events, RunningState* running_state)
{
    Gtk::TreeView* tree_view;
    builder->get_widget("running_machines_tree_view", tree_view);

    list_view_controller = new ListViewController(tree_view);
    this->running_state = running_state;

    running_state->select_machine.connect(sigc::mem_fun(this, &MachinesListController::on_machine_select));
    list_view_controller->selection_changed_details.connect(sigc::mem_fun(this, &MachinesListController::on_selection_changed));
}


void MachinesListController::on_machine_select(int machine_id, Machine* machine_def)
{
    _update_list(machine_id);
}


void MachinesListController::on_selection_changed(unsigned int machine_id, bool from_user)
{
    if(!from_user)
        return;

    running_state->user_select_machine(machine_id);
}


void MachinesListController::_update_list(int machine_id)
{
    vector<RunningMachine>& running_machines = running_state->get_running_machines();
    list_view_controller->clear();

    for(int i=0; i<running_machines.size(); i++)
    {
        auto rm = running_machines[i];
        list_view_controller->add_item(rm.name, rm.id);
    }

    if(machine_id != -1)
        list_view_controller->select_item(machine_id);
}
