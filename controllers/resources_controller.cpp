#include <string>

#include "resources_controller.h"
#include "historymanager/operations/resource_ops.h"
#include "main.h"


using std::string;



ResourcesController::ResourcesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder, Gtk::Window* main_window) : TopController(history_manager)
{
    is_setting = false;
    this->main_window = main_window;
    current_project_info = &((MainWindow*)main_window)->project_info;

    builder->get_widget("resource_tree_view", resource_tree_view);
    builder->get_widget("create_resource_button", create_button);
    builder->get_widget("delete_resource_button", delete_button);
    builder->get_widget("resource_name_label", name_label);
    builder->get_widget("resource_path_label", path_label);
    builder->get_widget("resource_path_file_chooser", path_file_chooser);
    builder->get_widget("resource_name_entry", name_entry);

    list_view_controller = new ListViewController(resource_tree_view);
    list_view_controller->selection_changed_signal.connect(sigc::mem_fun(this, &ResourcesController::on_selection_changed));

    name_entry->signal_changed().connect(sigc::mem_fun(this, &ResourcesController::on_name_changed));
    path_file_chooser->signal_file_set().connect(sigc::mem_fun(this, &ResourcesController::on_path_changed));

    create_button->signal_clicked().connect(sigc::mem_fun(this, &ResourcesController::on_create_clicked));
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &ResourcesController::on_delete_clicked));

    signals.model_changed.connect(sigc::mem_fun(this, &ResourcesController::on_model_changed));
    signals.project_load.connect(sigc::mem_fun(this, &ResourcesController::on_project_path_set));
    signals.project_save.connect(sigc::mem_fun(this, &ResourcesController::on_project_path_set));
}


void ResourcesController::on_path_changed()
{
    if(is_setting)
        return;

    string path = string(path_file_chooser->get_filename());
    string project_folder = current_project_info->project_directory;
    string relative = make_relative_path(project_folder, path);
    auto op = OpResourcePath(selected_resource, relative);
    history_manager->submit_operation(op);
}


void ResourcesController::on_name_changed()
{
    if(is_setting)
        return;

    string name = string(name_entry->get_text());
    auto op = OpResourceName(selected_resource, name);
    history_manager->submit_operation(op);
}


void ResourcesController::on_create_clicked()
{
    auto op = OpResourceCreate("new resource", "");
    unsigned int new_resource_id = history_manager->submit_operation(op);
    list_view_controller->select_item(new_resource_id);
}


void ResourcesController::on_delete_clicked()
{
    int resource_count = 0;

    auto machines = &history_manager->current_project.machines;
    for(int i = 0; i < machines->size(); i++)
    {
        auto locks = &((*machines)[i]->resourcelocks);
        for(int j = 0; j < locks->size(); j++)
        {
            auto lock = (*locks)[j];
            if(lock->resource == selected_resource)
                resource_count++;
        }
    }

    if(resource_count > 0)
    {
        Gtk::MessageDialog dialog(*main_window, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("This resource is currently used by " + std::to_string(resource_count) + " resource locks. Deleting the resource will delete the locks. Do you want to continue?");

        if(dialog.run() != Gtk::RESPONSE_OK)
            return;
    }

    auto op = OpResourceDelete(selected_resource);
    history_manager->submit_operation(op);
}


void ResourcesController::on_selection_changed(unsigned int entity_id)
{
    if(is_setting)
        return;

    is_setting = true;

    selected_resource = history_manager->current_project.get_resource_by_id(entity_id);
    selected_resource_id = selected_resource == nullptr ? 0 : entity_id;

    if(selected_resource == nullptr)
    {
        name_entry->set_text("");
        _reset_file_path();
    }
    
    else
    {
        name_entry->set_text(selected_resource->name);
        string project_folder = current_project_info->project_directory;
        string full_path = make_absolute_path(project_folder, selected_resource->path);
        path_file_chooser->set_filename(full_path);
    }

    is_setting = false;
    _update_enabled();
}


void ResourcesController::reset(bool reload)
{
    num_items = 0;
    selected_resource = nullptr;

    if(!reload)
        selected_resource_id = 0;

    is_setting = true;
    list_view_controller->clear();
    name_entry->set_text("");
    _reset_file_path();
    is_setting = false;

    _update_enabled();
}


void ResourcesController::load_from(Project& current_project, bool reload)
{
    _rebuild_treeview(0);
    _update_enabled();

    if(selected_resource_id != 0)
        list_view_controller->select_item(selected_resource_id);
}


void ResourcesController::on_project_path_set()
{
    if(selected_resource != nullptr && selected_resource->path != "")
        return;

    _reset_file_path();
}


void ResourcesController::on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type)
{
    if(entity_type != RESOURCE)
        return;

    if(signal_type == MODIFY)
    {
        // if modify, update inputs if it's selected, and change its name in the treeview
        if(selected_resource != nullptr && selected_resource->id == entity_id)
            on_selection_changed(entity_id);
        
        Resource* changed = history_manager->current_project.get_resource_by_id(entity_id);
        list_view_controller->change_item(changed->name, entity_id);        
        return;
    }

    // if it's a create/delete, just rebuild the tree view
    unsigned int deleted_id = signal_type == PRE_DELETE ? entity_id : 0;
    _rebuild_treeview(deleted_id);

    if(signal_type == CREATE || selected_resource_id != entity_id)
        return;

    selected_resource_id = 0;
    selected_resource = nullptr;
    on_selection_changed(0);
    _update_enabled();
}


void ResourcesController::_rebuild_treeview(unsigned int deleted_id)
{
    is_setting = true;
    list_view_controller->clear();
    
    auto resources = history_manager->current_project.resources;
    num_items = resources.size();

    for(int i = 0; i < num_items; i++)
    {
        if(resources[i]->id != deleted_id)
            list_view_controller->add_item(resources[i]->name, resources[i]->id);
    }
    
    is_setting = false;
    _update_enabled();
}


void ResourcesController::_update_enabled()
{
    bool resource_selected = selected_resource != nullptr;

    bool create_enabled = project_open;
    create_button->set_sensitive(create_enabled);

    bool modify_enabled = project_open && resource_selected;
    delete_button->set_sensitive(modify_enabled);
    name_entry->set_sensitive(modify_enabled);
    path_file_chooser->set_sensitive(modify_enabled);
    name_label->set_sensitive(modify_enabled);
    path_label->set_sensitive(modify_enabled);
}


void ResourcesController::_reset_file_path()
{
    string dir = current_project_info->project_directory;
    path_file_chooser->set_filename(dir);
}
