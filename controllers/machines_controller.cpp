#include "machines_controller.h"
#include "gui/gui_context.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/state_ops.h"
#include "historymanager/operations/transition_ops.h"
#include "historymanager/operations/resourcelock_ops.h"


MachinesController::MachinesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder, Gtk::Window* main_window) : TopController(history_manager)
{
    this->main_window = main_window;
    disable_input_signals = false;
    project_open = false;
    selected_machine = nullptr;
    selected_machine_id = 0;
    create_clicked = false;

    last_entity_id = 0;
    last_machine_id = 0;
    last_entity_type = NONE_ENTITY;

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

    gui_context = new GUIContext(gl_area, history_manager, GAM_BUILD);
    draw_context = &gui_context->gui_state.draw_context;

    list_view_controller = new ListViewController(tree_view, "Name:");
    list_view_controller->selection_changed_details.connect(sigc::mem_fun(this, &MachinesController::on_selection_changed));

    selected_resourcelock_ctrl = new SelectedResourceLock(history_manager, builder);
    selected_transition_ctrl = new SelectedTransition(history_manager, builder);
    selected_state_ctrl = new SelectedState(history_manager, builder);

    signals.focus_operation.connect(sigc::mem_fun(this, &MachinesController::on_focus_operation));
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


void MachinesController::on_focus_operation(Operation* operation, unsigned int result)
{
    int id = -1;
    
    if(is_instance<MachineChgOperation>(operation))
    {
        auto op = (MachineChgOperation*) operation;
        id = op->machine_id;
    }
    
    if(is_instance<OpMachineCreate>(operation))
        id = result;
    
    if(id == -1)
        return;
    
    if(selected_machine_id == id)
        return;
    
    list_view_controller->select_item(id);
    on_selection_changed(id, true);
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


void MachinesController::on_selection_changed(unsigned int machine_id, bool user_generated)
{
    disable_input_signals = true;
    
    if(user_generated)
        gui_context->unset_machine();

    if(machine_id == 0)
    {
        selected_machine = nullptr;
        selected_machine_id = 0;
        name_entry->set_text("");
        launch_on_start_switch->set_state(false);
    }

    else
    {
        auto current_project = &history_manager->current_project;
        selected_machine = current_project->get_machine_by_id(machine_id);
        _populate_ui_from_selected();

        if(user_generated)
            gui_context->set_machine(current_project, selected_machine);
    }

    if(user_generated)
        selected_machine_id = selected_machine == nullptr ? 0 : selected_machine->id;

    gui_context->update(); 
    disable_input_signals = false;
    _update_enabled();
}


void MachinesController::_populate_ui_from_selected()
{
    name_entry->set_text(selected_machine->name);
    launch_on_start_switch->set_state(selected_machine->run_on_start);
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
    
    for(int i = 0; i < machines.size(); i++)
    {
        if(machines[i]->id != deleted_id)
            list_view_controller->add_item(machines[i]->name, machines[i]->id);
    }
}


void MachinesController::load_from(Project& current_project, bool reload)
{
    _rebuild_list(0);
    _update_enabled();

    if(selected_machine_id != 0)
        list_view_controller->select_item(selected_machine_id);

    gui_context->load_from(current_project, reload);

    if(!reload)
        return;

    Machine* last_machine = current_project.get_machine_by_id(last_machine_id);
    Entity* last_entity = last_machine == nullptr ? nullptr : last_machine->get_entity_by_id(last_entity_id);
    last_entity_type = last_entity == nullptr ? NONE_ENTITY : last_entity_type; 

    _do_select(last_machine, last_entity_type, last_entity);

    if(reload && last_entity != nullptr)
        gui_context->restore_selected_entity(last_entity);
}


void MachinesController::reset(bool reload)
{
    gui_context->reset(reload);

    if(!reload)
    {
        last_entity_id = 0;
        last_machine_id = 0;
        last_entity_type = NONE_ENTITY;
    }

    list_view_controller->clear();
    _update_enabled();
}


void MachinesController::on_create_clicked()
{
    auto op = OpMachineCreate("new machine");
    unsigned int new_id = history_manager->submit_operation(op);
    selected_machine_id = new_id;
    list_view_controller->select_item(new_id);
    auto current_project = &history_manager->current_project;
    Machine* machine = current_project->get_machine_by_id(selected_machine_id);
    gui_context->set_machine(current_project, machine);
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
    Gtk::MessageDialog dialog(*main_window, "Pick Resource", false, Gtk::MESSAGE_OTHER, Gtk::BUTTONS_OK_CANCEL);
    Gtk::Box* box = dialog.get_message_area();
    Gtk::Box hbox(Gtk::ORIENTATION_HORIZONTAL);
    
    Gtk::Label resource_label;
    resource_label.set_markup("<b>resource:</b>");
    set_margins(&resource_label, 4);
    resource_label.set_margin_bottom(0);
    left_align(&resource_label);

    Gtk::ComboBoxText combobox;
    string first_resource;
    
    for(int i = 0; i < history_manager->current_project.resources.size(); i++)
    {
        Resource* res = history_manager->current_project.resources[i];
        combobox.append(res->name);
        if(i == 0)
            first_resource = res->name;
    }
    
    combobox.set_active_text(first_resource); 
    set_margins(&combobox, 4);
    combobox.set_margin_bottom(0);
    right_align(&combobox);
    
    hbox.pack_start(resource_label, false, false, 0);
    hbox.pack_start(combobox, false, false, 0);
    box->pack_start(hbox, false, false, 0);

    hbox.show();
    combobox.show();
    resource_label.show();

    if(dialog.run() != Gtk::RESPONSE_OK)
        return;

    string chosen = combobox.get_active_text();
    

    Resource* resource; 
    for(int i = 0; i < history_manager->current_project.resources.size(); i++)
    {
        resource = history_manager->current_project.resources[i];
        if(resource->name == chosen)
            break;
    }

    float wx, wy;
    new_entity_coords(wx, wy, false);
    create_clicked = true;
    auto op = OpResourceLockCreate(selected_machine, resource, wx, wy);
    history_manager->submit_operation(op);
    create_clicked = false;
}


void MachinesController::on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type)
{
    if(entity_type != MACHINE)
    {
        if(entity_type == RESOURCE)  // create resource lock button depends on resource existance
            _update_enabled();

        return;
    }

    if(signal_type == MODIFY)
    {
        // if a machine was changed, update in list and re-populate ui inputs if changed was selected.
        Machine* machine = history_manager->current_project.get_machine_by_id(entity_id);
        list_view_controller->change_item(machine->name, entity_id);
    
        if(entity_id == selected_machine_id)
            _populate_ui_from_selected();

        return;
    }

    // otherwise, rebuild list
    unsigned int to_delete_id = signal_type == PRE_DELETE ? entity_id : 0;
    unsigned int to_select_id = selected_machine == nullptr ? 0 : selected_machine->id;
    _rebuild_list(to_delete_id);

    // kinda hacky, TODO maybe figure out how to make less hacky?
    // if machine is created when user clicks "redo", this will select it
    // after the list view was re-built 
    if(!create_clicked && to_select_id != 0)
        list_view_controller->select_item(to_select_id);
}


void MachinesController::on_model_selected(Machine* machine, EntityType entity_type, Entity* entity)
{
    if(machine != selected_machine)
        return;
    
    last_entity_id = entity == nullptr ? 0 : entity->id;
    last_machine_id = machine->id;
    last_entity_type = entity_type;

    _do_select(machine, entity_type, entity);
}


void MachinesController::_do_select(Machine* machine, EntityType entity_type, Entity* entity)
{
    if(entity_type == NONE_ENTITY)
        selected_item_stack->set_visible_child("none_selected");

    else if(entity_type == STATE)
        selected_item_stack->set_visible_child("state_selected");

    else if(entity_type == TRANSITION)
        selected_item_stack->set_visible_child("transition_selected");

    else if(entity_type == RESOURCELOCK)
        selected_item_stack->set_visible_child("resourcelock_selected");

    selected_resourcelock_ctrl->set_selected(machine, entity);
    selected_transition_ctrl->set_selected(machine, entity);
    selected_state_ctrl->set_selected(machine, entity);
}
