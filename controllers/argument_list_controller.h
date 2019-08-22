#pragma once

#include <vector>

#include "models/datatypes/arg.h"
#include "controllers/value_input_controller.h"
#include "controllers/list_view_controller.h"

using std::vector;


class ArgumentListController
{
/*

   tree view

    argname: ____
    argtype: [ v]
    value:   ____

    [creaete] [delete]

*/
    public:
        ArgumentListController();

        void attach(Gtk::Container* parent_container);

        sigc::signal<void, vector<Arg>> on_args_changed;
        void set_args(vector<Arg> args);

    private:
        vector<Arg> arg_list;

        Gtk::Grid main_grid;
        Gtk::Box value_input_box;
        Gtk::TreeView tree_view;
        Gtk::Button create_button, delete_button;
        Gtk::Entry arg_name_entry;
        Gtk::Label arg_name_label, title_label;

        ValueInputController* value_input_controller;
        ListViewController* list_view_controller;

        bool is_emitting, is_setting;
        int selected_index;

        void _update();
        void _fire_signal();
        void _update_selected();

        void on_create_clicked();
        void on_delete_clicked();
        void on_name_changed();
        void on_value_changed(LVOV value);
        void on_selection_changed(unsigned int index);
};

