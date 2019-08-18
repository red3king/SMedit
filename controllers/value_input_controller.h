#pragma once

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <string>
#include <vector>

#define SVT_ANY "any"
#define SVT_STR "string"
#define SVT_INT "int"
#define SVT_FLOAT "float"
#define SVT_REF "reference"


using std::string;
using std::vector;


enum ValueType { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };
static const ValueType AllValueTypes[] = { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };


string value_type_to_string(ValueType vt);
ValueType string_to_value_type(string str);


/*

    return - literal or variable
    spawn - literal or variable, must be string (name of task) | literal or variable ( arg ) | variable  ( pid storage)
    join - variable (pid) | variable (for result)
    run - literal or variable, string (name of task) | literal or variable (arg) | variable (result)
   
    
   
   
   
*/


class LVOV
{
    // Literal Value or Variable name
    public:
        LVOV();
        LVOV(string str_val);
        LVOV(string str_val, bool is_variable);
        LVOV(int int_val);
        LVOV(float float_val);

        void reset();

        bool from_string(Glib::ustring input); // attempt to update value from string

        // Returns string representation of value/varname 
        string to_string();

        ValueType type;

        string variable_name;
        string str_val;
        int int_val;
        float float_val;

        // These operators return the underlying value or variable name
        operator int() const;
        operator string() const;
        operator float() const;
};


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
