#pragma once

#include <gtkmm.h>

#include "selected_item.h"
#include "controllers/value_input_controller.h"
#include "selected_state/initial_state_ctrl.h"
#include "selected_state/code_state.h"
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

        ValueInputController* return_value_input;
        InitialStateController* initial_state_ctrl;
        CodeStateController* code_state_ctrl;

        // widgets
        Gtk::Button* delete_button;
        Gtk::Label* title_label;
        Gtk::Entry* name_entry;
        Gtk::ComboBoxText* type_combobox;
        Gtk::Stack* state_types_stack;

        Gtk::Entry* join_pidvar_entry;

        void on_name_changed();
        void on_type_changed();
        void on_delete_clicked();

        void on_jpidvar_changed();
        void on_retval_changed(LVOV lvov);
        void on_initial_state_changed(vector<ArgDef> arguments);
        void on_code_changed(string code);

        void init_initial_state_ctrl(Glib::RefPtr<Gtk::Builder> const& builder);
};
