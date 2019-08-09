#include <string>

#include "selected_resourcelock.h"
#include "historymanager/operations/resourcelock_ops.h"


SelectedResourceLock::SelectedResourceLock(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : SelectedItemController(history_manager)
{
    selected_resourcelock = nullptr;
    builder->get_widget("resourcelock_selected_name", selected_resourcelock_name);
    builder->get_widget("resourcelock_selected_delete", selected_resourcelock_delete);

    selected_resourcelock_delete->signal_clicked().connect(sigc::mem_fun(this, &SelectedResourceLock::on_delete_clicked));
}


void SelectedResourceLock::set_selected_impl(Entity* entity)
{
    selected_resourcelock = dynamic_cast<ResourceLock*>(entity);
}


void SelectedResourceLock::on_delete_clicked()
{
    auto op = OpResourceLockDelete(owning_machine, selected_resourcelock);
    history_manager->submit_operation(op);
}


void SelectedResourceLock::update()
{
    string text;
    bool enabled;

    if(selected_resourcelock == nullptr)
    {
        text = "";
        enabled = false;
    }

    else
    {
        text = "Lock (" + selected_resourcelock->resource->name + ")";
        enabled = true;
    }

    selected_resourcelock_name->set_text(text);
    selected_resourcelock_delete->set_sensitive(enabled);
}


Entity* SelectedResourceLock::get_entity()
{
    return (Entity*)selected_resourcelock;
}
