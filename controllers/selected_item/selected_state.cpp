#include "selected_state.h"
#include "historymanager/operations/state_ops.h"


SelectedState::SelectedState(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : SelectedItemController(history_manager)
{
    builder->get_widget("delete_state_button", delete_button);
    builder->get_widget("selected_state_title", title_label);
    builder->get_widget("state_name_entry", name_entry);
    builder->get_widget("state_type_combobox", type_combobox);    

    Gtk::Box* selected_state_grid;
    builder->get_widget("test_box", selected_state_grid);
    // value_input = new ValueInputController(selected_state_grid, "tvi");

    name_entry->signal_changed().connect(sigc::mem_fun(this, &SelectedState::on_name_changed));
    type_combobox->signal_changed().connect(sigc::mem_fun(this, &SelectedState::on_type_changed));
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &SelectedState::on_delete_clicked));
}


void SelectedState::on_name_changed()
{
    auto op = OpStateName(owning_machine, selected_state, name_entry->get_text());
    history_manager->submit_operation(op);
}


void SelectedState::on_type_changed()
{
    StateType type = string_to_state_type(type_combobox->get_active_text());
    auto op = OpStateType(owning_machine, selected_state, type);
    history_manager->submit_operation(op);
}


void SelectedState::on_delete_clicked()
{
    auto op = OpStateDelete(owning_machine, selected_state);
    history_manager->submit_operation(op);
}


void SelectedState::set_selected_impl(Entity* entity)
{
    selected_state = dynamic_cast<State*>(entity);
}


Entity* SelectedState::get_entity()
{
    return (Entity*)selected_state;
}


void SelectedState::update()
{
    if(selected_state == nullptr)
        return;

    title_label->set_text("State " + std::to_string(selected_state->id));
    name_entry->set_text(selected_state->name);
    type_combobox->set_active_text(state_type_to_string(selected_state->type));
    
}
