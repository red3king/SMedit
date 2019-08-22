#include <iostream>

#include "selected_transition.h"
#include "utils.h"
#include "historymanager/operations/transition_ops.h"



SelectedTransition::SelectedTransition(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : SelectedItemController(history_manager)
{
    is_overwriting_user = false;
    selected_transition = nullptr;

    builder->get_widget("transition_selected_delete", delete_button);
    builder->get_widget("transition_selected_title", name_label);
    builder->get_widget("event_timeout_label", event_timeout_label);
    builder->get_widget("transition_type_combobox", transition_type_combobox);
    builder->get_widget("event_timeout_entry", event_timeout_entry);
    
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &SelectedTransition::on_delete_clicked));
    transition_type_combobox->signal_changed().connect(sigc::mem_fun(this, &SelectedTransition::on_tt_combobox_changed));
    event_timeout_entry->signal_changed().connect(sigc::mem_fun(this, &SelectedTransition::on_event_timeout_changed));
}


void SelectedTransition::on_event_timeout_changed()
{
    if(is_overwriting_user)
        return;

    auto text = event_timeout_entry->get_text();

    if(selected_transition->type == TIMEOUT)
    {
        float value = selected_transition->timeout;
        bool valid = string_to_float(value, text);
        
        if(!valid)
        {
            is_overwriting_user = true;
            event_timeout_entry->set_text(std::to_string(selected_transition->timeout));  
            is_overwriting_user = false;
            return;
        }

        auto op = OpTransitionTimeout(owning_machine, selected_transition, value);
        history_manager->submit_operation(op);
    }

    else if(selected_transition->type == EVENT)
    {
        auto op = OpTransitionEvName(owning_machine, selected_transition, text);
        history_manager->submit_operation(op);
    }
}


void SelectedTransition::on_tt_combobox_changed()
{
    auto type_str = transition_type_combobox->get_active_text();

    TransitionType type = CATCHALL;

    if(type_str == "Event")
        type = EVENT;
    else if(type_str == "Timeout")
        type = TIMEOUT;

    auto op = OpTransitionType(owning_machine, selected_transition, type);
    history_manager->submit_operation(op);
}


void SelectedTransition::on_delete_clicked()
{
    auto op = OpTransitionDelete(owning_machine, selected_transition);
    history_manager->submit_operation(op);
}


void SelectedTransition::set_selected_impl(Machine* owning_machine, Entity* entity)
{
    selected_transition = dynamic_cast<Transition*>(entity);
}


Entity* SelectedTransition::get_entity()
{
    return (Entity*)selected_transition;
}


void SelectedTransition::update()
{
    if(selected_transition == nullptr)
        return;

    name_label->set_text("Transition " + std::to_string(selected_transition->id));

    if(selected_transition->type == CATCHALL)
    {
        transition_type_combobox->set_active_text("Catchall");
        event_timeout_label->hide();
        event_timeout_entry->hide();
    }

    else
    {
        if(selected_transition->type == EVENT)
        {
            transition_type_combobox->set_active_text("Event");
            event_timeout_label->set_text("Event:");
            event_timeout_entry->set_text(selected_transition->event_name);
        }

        else if(selected_transition->type == TIMEOUT)
        {
            transition_type_combobox->set_active_text("Timeout");
            event_timeout_label->set_text("Timeout: ");
            event_timeout_entry->set_text(std::to_string(selected_transition->timeout));
        }

        event_timeout_label->show();
        event_timeout_entry->show();
    }
}
