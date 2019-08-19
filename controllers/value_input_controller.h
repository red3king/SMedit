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
    /*
    

      TITILE
      variable or value? [\/ variable]
      value type         [\/ any ]
      variable name      "asdfasdasd" 
     
       */

    public:
        ValueInputController(Gtk::Container *parent_container, string name);  // allows all value types
        ValueInputController(Gtk::Container *parent_container, vector<ValueType> allowed_types, string name);
    
        sigc::signal<void, LVOV> value_changed_signal;

        void set_value(LVOV new_value);
        LVOV get_value();

        void on_value_type_changed();
        void on_value_changed();

    private:
        void _init(Gtk::Container *parent_container, vector<ValueType> allowed_types, string name);
        void _update();

        LVOV value;
        bool is_updating;

        Gtk::ComboBoxText value_type_combobox;
        Gtk::Entry value_entry;
        Gtk::Label title_label, value_type_label, value_label;
        Gtk::Grid container_grid;
};
