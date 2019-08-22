#pragma once

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <string>
#include <vector>

#include "models/datatypes/lvov.h"


using std::string;
using std::vector;


class ValueInputController
{
    public:
        ValueInputController(); // no name label
        ValueInputController(string name);  // allows all value types
        ValueInputController(vector<ValueType> allowed_types, string name);

        void attach(Gtk::Container *parent_container);

        sigc::signal<void, LVOV> value_changed_signal;

        void set_value(LVOV new_value);
        LVOV get_value();

        void on_value_type_changed();
        void on_value_changed();

        void set_enabled(bool enabled);

    private:
        void _init(vector<ValueType> allowed_types, bool has_name, string name);

        LVOV value;
        bool is_updating;
        bool has_name;

        Gtk::ComboBoxText value_type_combobox;
        Gtk::Entry value_entry;
        Gtk::Label title_label, value_type_label, value_label;
        Gtk::Grid container_grid;
};
