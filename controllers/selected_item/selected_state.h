#pragma once

#include <gtkmm.h>

#include "selected_item.h"
#include "controllers/value_input_controller.h"
#include "models/state.h"


class SelectedState : public SelectedItemController
{
    public:
        SelectedState(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

        void set_selected_impl(Entity* entity);
        Entity* get_entity();
        void update();

    private:
        State* selected_state;

        //ValueInputController* value_input;

        // widgets
        Gtk::Button* delete_button;
        Gtk::Label* title_label;
        Gtk::Entry* name_entry;
        Gtk::ComboBoxText* type_combobox;

        void on_name_changed();
        void on_type_changed();
        void on_delete_clicked();
};
