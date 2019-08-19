#pragma once

#include <vector>
#include "controllers/list_view_controller.h"
#include "models/datatypes/argdef.h"

using std::vector;


class InitialStateController
{
    public:
        InitialStateController(Gtk::TreeView* tree_view, Gtk::Button* delete_button, Gtk::Button* create_button,
                Gtk::Entry* arg_name_entry, Gtk::ComboBoxText* arg_type_combobox);

        sigc::signal<void, vector<ArgDef>> config_changed_signal;

        void set_config(vector<ArgDef> arguments);
        vector<ArgDef> get_config();

    private:
        bool is_updating, is_emitting;
        int selected_row;

        ListViewController* list_view_controller;

        Gtk::Entry* arg_name_entry;
        Gtk::ComboBoxText* arg_type_combobox;
        Gtk::Button* delete_button;

        vector<ArgDef> arguments;

        void _fire_event();
        void _update();
        void update_gridview(unsigned int row);

        void on_arg_name_changed();
        void on_arg_type_changed();
        void on_listview_selection_changed(unsigned int selection);
        void on_delete_clicked();
        void on_create_clicked();
};


