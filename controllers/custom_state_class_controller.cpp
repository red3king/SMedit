#include <string>

#include "custom_state_class_controller.h"
#include "historymanager/operations/custom_state_ops.h"
#include "main.h"

using std::string;


CustomStateClassController::CustomStateClassController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder, Gtk::Window* main_window) : TopController(history_manager)
{
    is_setting = false;
    this->main_window = main_window;
    current_project_info = &((MainWindow*)main_window)->project_info;

    builder->get_widget("csc_tree_view", csc_tree_view);
    builder->get_widget("create_csc_button", create_button);
    builder->get_widget("delete_csc_button", delete_button);
    builder->get_widget("reload_csc_button", reload_button);
    builder->get_widget("csc_name_label", name_label);
    builder->get_widget("csc_path_label", path_label);
    builder->get_widget("csc_path_file_chooser", path_file_chooser);
    builder->get_widget("csc_name_entry", name_entry);
    builder->get_widget("csc_preview", preview_label);
    
    list_view_controller = new ListViewController(csc_tree_view);
    list_view_controller->selection_changed_signal.connect(sigc::mem_fun(this, &CustomStateClassController::on_selection_changed));

    name_entry->signal_changed().connect(sigc::mem_fun(this, &CustomStateClassController::on_name_changed));
    path_file_chooser->signal_file_set().connect(sigc::mem_fun(this, &CustomStateClassController::on_path_changed));

    create_button->signal_clicked().connect(sigc::mem_fun(this, &CustomStateClassController::on_create_clicked));
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &CustomStateClassController::on_delete_clicked));
    reload_button->signal_clicked().connect(sigc::mem_fun(this, &CustomStateClassController::on_reload_clicked));
    
    signals.focus_operation.connect(sigc::mem_fun(this, &CustomStateClassController::on_focus_operation));
    signals.model_changed.connect(sigc::mem_fun(this, &CustomStateClassController::on_model_changed));
    signals.project_save.connect(sigc::mem_fun(this, &CustomStateClassController::on_project_path_set));
    signals.project_load.connect(sigc::mem_fun(this, &CustomStateClassController::on_project_path_set));
}


void CustomStateClassController::_build_preview()
{
    if(selected_csc == nullptr)
    {
        preview_label->set_text("");
        return;
    }
    
    string preview_text = "";
    
    for(int i = 0; i < selected_csc->transition_defs.size(); i++)
    {
        preview_text += "\r\n";
        auto def = selected_csc->transition_defs[i];
        
        if(def.type == EVENT)
            preview_text += "event (" + def.event_name + ")";
        
        else if(def.type == TIMEOUT)
            preview_text += "timeout (" + std::to_string(def.timeout) + "s)";
        
        else if(def.type == CATCHALL)
            preview_text += "* catchall";
    }
    
    preview_label->set_text(preview_text);
}


void CustomStateClassController::on_focus_operation(Operation* operation, unsigned int result)
{
    if(!is_instance<OpCustStateCreate>(operation))
        return;
    
    list_view_controller->select_item(result);
    on_selection_changed(result);
}


void CustomStateClassController::on_path_changed()
{
    if(is_setting)
        return;

    string path = string(path_file_chooser->get_filename());
    string project_folder = current_project_info->project_directory;
    string relative = make_relative_path(project_folder, path);
    auto op = OpCustomStateClassPath(selected_csc, relative);
    history_manager->submit_operation(op);
}


void CustomStateClassController::on_name_changed()
{
    if(is_setting)
        return;

    string name = string(name_entry->get_text());
    auto op = OpCustomStateClassName(selected_csc, name);
    history_manager->submit_operation(op);
}


void CustomStateClassController::on_create_clicked()
{
    auto op = OpCustStateCreate("custom_state_class", "");
    unsigned int new_csc_id = history_manager->submit_operation(op);
    list_view_controller->select_item(new_csc_id);
}


void CustomStateClassController::on_reload_clicked()
{
    CustomStateClass loaded_class;
    string error_msg;
    string project_folder = current_project_info->project_directory;
    string absolute_path = make_absolute_path(project_folder, selected_csc->path);
    
    if(!load_csc_from_file(loaded_class, absolute_path, error_msg))
    {
        display_error("could not load / execute given file:\r\n" + error_msg);
        return;
    }
    
    auto op = OpCustomStateClassReload(selected_csc, loaded_class);
    history_manager->submit_operation(op);
}


int count_dependent_states(vector<Machine*>& machines, unsigned int selected_csc_id)
{
    // Count and/or delete all states of given custom state class
    int csc_count = 0;

    for(int i = 0; i < machines.size(); i++)
    {
        Machine* machine = machines[i];
        
        for(int j = 0; j < machine->states.size(); j++)
        {
            auto state = machine->states[j];
            
            if(!state->is_custom() || state->type != selected_csc_id)
                continue;
            
            csc_count++;
        }
    }        
    
    return csc_count;
}


void CustomStateClassController::on_delete_clicked()
{
    int csc_count = count_dependent_states(history_manager->current_project.machines, selected_csc_id);

    if(csc_count > 0)
    {
        Gtk::MessageDialog dialog(*main_window, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("This custom state class is currently used by " + std::to_string(csc_count) + " states. Deleting this class will delete the states. Do you want to continue?");

        if(dialog.run() != Gtk::RESPONSE_OK)
            return;
    }

    auto op = OpCustStateDelete(selected_csc); // operation takes care of deleting dependent states
    history_manager->submit_operation(op);
}


void CustomStateClassController::on_selection_changed(unsigned int entity_id)
{
    if(is_setting)
        return;

    is_setting = true;

    selected_csc = history_manager->current_project.get_custom_state_class_by_id(entity_id);
    selected_csc_id = selected_csc == nullptr ? 0 : entity_id;

    if(selected_csc == nullptr)
    {
        name_entry->set_text("");
        path_file_chooser->set_filename("");
    }
    
    else
    {
        name_entry->set_text(selected_csc->name);
        string project_folder = current_project_info->project_directory;
        string full_path = make_absolute_path(project_folder, selected_csc->path);
        path_file_chooser->set_filename(full_path);
    }

    is_setting = false;
    _update_enabled();
}


void CustomStateClassController::reset(bool reload)
{
    num_items = 0;
    selected_csc = nullptr;

    if(!reload)
        selected_csc_id = 0;

    is_setting = true;
    list_view_controller->clear();
    name_entry->set_text("");
    path_file_chooser->set_filename("");
    is_setting = false;
    _update_enabled();
}


void CustomStateClassController::load_from(Project& current_project, bool reload)
{
    _rebuild_treeview(0);
    _update_enabled();

    if(selected_csc_id != 0)
        list_view_controller->select_item(selected_csc_id);
}


void CustomStateClassController::on_project_path_set()
{
    if(selected_csc != nullptr && selected_csc->path != "")
        return;
    
    string folder = current_project_info->project_directory;
    path_file_chooser->set_filename(folder);
}


void CustomStateClassController::on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type)
{
    if(entity_type != CUSTOM_STATE_CLASS)
        return;

    if(signal_type == MODIFY)
    {
        // if modify, update inputs if it's selected, and change its name in the treeview
        if(selected_csc != nullptr && selected_csc->id == entity_id)
            on_selection_changed(entity_id);
        
        CustomStateClass* changed = history_manager->current_project.get_custom_state_class_by_id(entity_id);
        list_view_controller->change_item(changed->name, entity_id);        
        return;
    }

    // if it's a create/delete, just rebuild the tree view
    unsigned int deleted_id = signal_type == PRE_DELETE ? entity_id : 0;
    _rebuild_treeview(deleted_id);

    if(signal_type == CREATE || selected_csc_id != entity_id)
        return;

    selected_csc_id = 0;
    selected_csc = nullptr;
    on_selection_changed(0);
    _update_enabled();
}


void CustomStateClassController::_rebuild_treeview(unsigned int deleted_id)
{
    is_setting = true;
    list_view_controller->clear();
    
    auto cscs = history_manager->current_project.custom_state_classes;
    num_items = cscs.size();

    for(int i = 0; i < num_items; i++)
    {
        if(cscs[i]->id != deleted_id)
            list_view_controller->add_item(cscs[i]->name, cscs[i]->id);
    }
    
    is_setting = false;
    _update_enabled();
}


void CustomStateClassController::_update_enabled()
{
    bool csc_selected = selected_csc != nullptr;

    bool create_enabled = project_open;
    create_button->set_sensitive(create_enabled);

    bool modify_enabled = project_open && csc_selected;
    reload_button->set_sensitive(modify_enabled);
    delete_button->set_sensitive(modify_enabled);
    name_entry->set_sensitive(modify_enabled);
    path_file_chooser->set_sensitive(modify_enabled);
    name_label->set_sensitive(modify_enabled);
    path_label->set_sensitive(modify_enabled);
    
     _build_preview();
}
