#include "machines_controller.h"
#include "gui/gui_context.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/state_ops.h"
#include "historymanager/operations/transition_ops.h"
#include "historymanager/operations/resourcelock_ops.h"


MachinesController::MachinesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder)
{
    this->history_manager = history_manager;

    disable_input_signals = false;
    project_open = false;
    selected_machine = nullptr;

    builder->get_widget("machine_tree_view", tree_view);
    builder->get_widget("create_machine_button", create_button);
    builder->get_widget("delete_machine_button", delete_button);
    builder->get_widget("new_state_button", create_state_button);
    builder->get_widget("new_transition_button", create_transition_button);
    builder->get_widget("new_resourcelock_button", create_resourcelock_button);
    builder->get_widget("machine_name_label", name_label);
    builder->get_widget("machine_launch_on_start_label", launch_on_start_label);
    builder->get_widget("machine_name_entry", name_entry);
    builder->get_widget("machine_launch_on_start_switch", launch_on_start_switch);
    builder->get_widget("selected_item_stack", selected_item_stack);
    builder->get_widget("machine_edit_gl_area", gl_area);

    gui_context = new GUIContext(gl_area, history_manager);
    draw_context = &gui_context->gui_state.draw_context;

    list_view_controller = new ListViewController(tree_view);
    list_view_controller->selection_changed_signal.connect(sigc::mem_fun(this, &MachinesController::on_selection_changed));

    selected_resourcelock_ctrl = new SelectedResourceLock(history_manager, builder);

    signals.project_open.connect(sigc::mem_fun(this, &MachinesController::on_project_open));
    signals.project_close.connect(sigc::mem_fun(this, &MachinesController::on_project_close));
    signals.model_changed.connect(sigc::mem_fun(this, &MachinesController::on_model_changed));
    signals.model_selected.connect(sigc::mem_fun(this, &MachinesController::on_model_selected));

    create_button->signal_clicked().connect(sigc::mem_fun(this, &MachinesController::on_create_clicked));
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &MachinesController::on_delete_clicked));
    create_transition_button->signal_clicked().connect(sigc::mem_fun(this, &MachinesController::on_create_transition_clicked));
    create_state_button->signal_clicked().connect(sigc::mem_fun(this, &MachinesController::on_create_state_clicked));
    create_resourcelock_button->signal_clicked().connect(sigc::mem_fun(this, &MachinesController::on_create_resourcelock_clicked));

    name_entry->signal_changed().connect(sigc::mem_fun(this, &MachinesController::on_name_changed));
    launch_on_start_switch->property_active().signal_changed().connect(sigc::mem_fun(this, &MachinesController::on_launch_on_start_changed));
}


void MachinesController::on_name_changed()
{
    if(disable_input_signals)
        return;

    string name = string(name_entry->get_text());
    auto op = OpMachineName(selected_machine, name);
    history_manager->submit_operation(op);
}


void MachinesController::on_launch_on_start_changed()
{
    if(disable_input_signals)
        return;

    bool launch_on_start = launch_on_start_switch->get_state();
    auto op = OpMachineRunOnStart(selected_machine, launch_on_start);
    history_manager->submit_operation(op);
}


void MachinesController::new_entity_coords(float& x, float& y, bool offset)
{
    float screen_x = 100;
    float screen_y = 100;

    if(offset)
    {
        screen_x += 100;
        screen_y += 100;
    }

    draw_context->screen_to_world(x, y, screen_x, screen_y);
}


void MachinesController::on_selection_changed(unsigned int machine_id)
{
    disable_input_signals = true;
    gui_context->unset_machine();

    if(machine_id == 0)
    {
        selected_machine = nullptr;
        name_entry->set_text("");
        launch_on_start_switch->set_state(false);
    }

    else
    {
        selected_machine = history_manager->current_project.get_machine_by_id(machine_id);
        name_entry->set_text(selected_machine->name);
        launch_on_start_switch->set_state(selected_machine->run_on_start);
        gui_context->set_machine(selected_machine);
    }
   
    gui_context->update(); 
    disable_input_signals = false;
    _update_enabled();
}


void MachinesController::_update_enabled()
{
    bool machine_selected = project_open && selected_machine != nullptr;
    bool resource_exists = history_manager->current_project.resources.size() > 0;

    create_button->set_sensitive(project_open);
    delete_button->set_sensitive(machine_selected);
    create_state_button->set_sensitive(machine_selected);
    create_transition_button->set_sensitive(machine_selected);
    create_resourcelock_button->set_sensitive(machine_selected && resource_exists);
    name_label->set_sensitive(machine_selected);
    launch_on_start_label->set_sensitive(machine_selected);
    name_entry->set_sensitive(machine_selected);
    launch_on_start_switch->set_sensitive(machine_selected);

}


void MachinesController::_rebuild_list(unsigned deleted_id)
{
    list_view_controller->clear();
    auto machines = history_manager->current_project.machines;
    for(int i=0; i<machines.size(); i++)
    {
        if(machines[i]->id != deleted_id)
            list_view_controller->add_item(machines[i]->name, machines[i]->id);
    }
}


void MachinesController::on_project_open()
{
    project_open = true;
    _rebuild_list(0);
    _update_enabled();
}


void MachinesController::on_project_close()
{
    project_open = false;
    list_view_controller->clear();
    _update_enabled();
}


void MachinesController::on_create_clicked()
{
    auto op = OpMachineCreate("new machine");
    unsigned int new_id = history_manager->submit_operation(op);
    list_view_controller->select_item(new_id);
}


void MachinesController::on_delete_clicked()
{
    auto op = OpMachineDelete(selected_machine);
    history_manager->submit_operation(op);
}


void MachinesController::on_create_state_clicked()
{
    float world_x, world_y;
    new_entity_coords(world_x, world_y, false);

    auto op = OpStateCreate(selected_machine, world_x, world_y);
    history_manager->submit_operation(op);
}


void MachinesController::on_create_transition_clicked()
{
    float wx0, wy0, wx1, wy1;
    new_entity_coords(wx0, wy0, false);
    new_entity_coords(wx1, wy1, true);

    auto op = OpTransitionCreate(selected_machine, wx0, wy0, wx1, wy1);
    history_manager->submit_operation(op);
}


void MachinesController::on_create_resourcelock_clicked()
{
    Resource* resource = history_manager->current_project.resources[0];
    float wx, wy;
    new_entity_coords(wx, wy, false);

    auto op = OpResourceLockCreate(selected_machine, resource, wx, wy);
    history_manager->submit_operation(op);
}


void MachinesController::on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(entity_type != MACHINE)
    {
        if(entity_type == RESOURCE)  // create resource lock button depends on resource existance
            _update_enabled();

        return;
    }

    if(signal_type == MODIFY)
    {
        // if a machine was changed, just update it in the list
        Machine* machine = history_manager->current_project.get_machine_by_id(entity_id);
        list_view_controller->change_item(machine->name, entity_id);
        return;
    }

    // otherwise, rebuild list
    unsigned int to_delete_id = signal_type == PRE_DELETE ? entity_id : 0;
    _rebuild_list(to_delete_id);
}


void MachinesController::on_model_selected(Machine* machine, EntityType entity_type, Entity* entity)
{
    if(machine != selected_machine)
        return;

    if(entity_type == NONE_ENTITY)
        selected_item_stack->set_visible_child("none_selected");

    else if(entity_type == STATE)
        selected_item_stack->set_visible_child("state_selected");

    else if(entity_type == TRANSITION)
        selected_item_stack->set_visible_child("transition_selected");

    else if(entity_type == RESOURCELOCK)
        selected_item_stack->set_visible_child("resourcelock_selected");

    selected_resourcelock_ctrl->set_selected(machine, entity);
}
