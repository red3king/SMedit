#pragma once

#include <gtkmm.h>

#include "selected_item.h"
#include "models/transition.h"


class SelectedTransition : public SelectedItemController
{
    public:
        SelectedTransition(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

        void set_selected_impl(Entity* entity);
        Entity* get_entity();
        void update();

    private:
        Transition* selected_transition;
        bool is_overwriting_user;

        // widgets
        Gtk::Button* delete_button;
        Gtk::Label* name_label;
        Gtk::Label* event_timeout_label;
        Gtk::ComboBoxText* transition_type_combobox;
        Gtk::Entry* event_timeout_entry;

        void on_delete_clicked();
        void on_tt_combobox_changed();
        void on_event_timeout_changed();
};
