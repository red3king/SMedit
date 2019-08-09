#include <string>

#include "resources_controller.h"
#include "historymanager/operations/resource_ops.h"

using std::string;


ResourcesController::ResourcesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder)
{
    project_open = false;
    disable_input_signals = false;
    selected_resource = nullptr;
    this->history_manager = history_manager;

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

    signals.project_open.connect(sigc::mem_fun(this, &ResourcesController::on_project_open));
    signals.project_close.connect(sigc::mem_fun(this, &ResourcesController::on_project_close));
    signals.model_changed.connect(sigc::mem_fun(this, &ResourcesController::on_model_changed));
}


void ResourcesController::on_path_changed()
{
    if(disable_input_signals)
        return;

    string path = string(path_file_chooser->get_filename());
    auto op = OpResourcePath(selected_resource, path);
    history_manager->submit_operation(op);
}


void ResourcesController::on_name_changed()
{
    if(disable_input_signals)
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
    auto op = OpResourceDelete(selected_resource);
    history_manager->submit_operation(op);
}


void ResourcesController::on_selection_changed(unsigned int entity_id)
{
    disable_input_signals = true;

    if(entity_id == 0)
    {
        selected_resource = nullptr;
        name_entry->set_text("");
        path_file_chooser->set_filename("");
    }
    
    else
    {
        selected_resource = history_manager->current_project.get_resource_by_id(entity_id);
        name_entry->set_text(selected_resource->name);
        path_file_chooser->set_filename(selected_resource->path);
    }

    disable_input_signals = false;
    _update_enabled();
}


void ResourcesController::on_project_open()
{
    project_open = true;
    _rebuild_treeview(0);
    _update_enabled();
}


void ResourcesController::on_project_close()
{
    project_open = false;
    num_items = 0;
    selected_resource = nullptr;
    list_view_controller->clear();
    _update_enabled();
}


void ResourcesController::on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
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
}


void ResourcesController::_rebuild_treeview(unsigned int deleted_id)
{
    list_view_controller->clear();
    
    auto resources = history_manager->current_project.resources;
    num_items = resources.size();

    for(int i=0; i<num_items; i++)
    {
        if(resources[i]->id != deleted_id)
            list_view_controller->add_item(resources[i]->name, resources[i]->id);
    }

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
