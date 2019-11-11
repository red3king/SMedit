#pragma once

#include <gtkmm.h>

#include "selected_item.h"
#include "controllers/value_input_controller.h"
#include "selected_state/initial_state_ctrl.h"
#include "selected_state/code_state.h"
#include "selected_state/spawn_state_ctrl.h"
#include "selected_state/return_state_ctrl.h"
#include "selected_state/join_state_ctrl.h"
#include "models/state.h"


class SelectedState : public SelectedItemController
{
    public:
        SelectedState(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

        void set_selected_impl(Machine* owning_machine, Entity* entity);
        Entity* get_entity();
        void update();

    private:
        State* selected_state;

        InitialStateController* initial_state_ctrl;
        CodeStateController* code_state_ctrl;
        SpawnStateController* spawn_state_ctrl;
        ReturnStateController* return_state_ctrl;
        JoinStateController* join_state_ctrl;

        // widgets
        Gtk::Button* delete_button;
        Gtk::Label* title_label;
        Gtk::Entry* name_entry;
        Gtk::ComboBoxText* type_combobox;
        Gtk::Stack* state_types_stack;

        void on_name_changed();
        void on_type_changed();
        void on_delete_clicked();

        void on_initial_state_changed(vector<ArgDef> arguments);
        void on_code_changed(string code);

        void init_initial_state_ctrl(Glib::RefPtr<Gtk::Builder> const& builder);
};
