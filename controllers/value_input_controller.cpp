#include <stdexcept>

#include "value_input_controller.h"
#include "utils.h"


string value_type_to_string(ValueType vt)
{
    switch(vt)
    {
        case VT_ANY:
            return SVT_ANY;       
        case VT_STR:
            return SVT_STR;
        case VT_INT:
            return SVT_INT;
        case VT_FLOAT:
            return SVT_FLOAT;
        case VT_REF:
            return SVT_REF;
    }

    throw std::invalid_argument("unknown ValueType");
}


ValueType string_to_value_type(string str)
{
 
    if(str == SVT_ANY)
        return VT_ANY;

    if(str == SVT_STR)
        return VT_STR;
        
    if(str == SVT_INT)
        return VT_INT;

    if(str == SVT_FLOAT)
        return VT_FLOAT;

    if(str == SVT_REF)
        return VT_REF;

    throw std::invalid_argument("unknown value string");
}


// Literal Value or Variable

LVOV::LVOV()
{
    reset();
}


LVOV::LVOV(string str_val)
{ 
    reset();
    this->str_val = str_val;
    type = VT_STR;
}


LVOV::LVOV(string str_val, bool is_variable)
{
    reset();
    
    if(is_variable)
    {
        type = VT_REF;
        variable_name = str_val;
    }

    else
    {
        type = VT_STR;
        this->str_val = str_val;
    }
}


LVOV::LVOV(int int_val)
{
    reset();
    this->int_val = int_val;
    type = VT_INT;
}


LVOV::LVOV(float float_val)
{
    reset();
    this->float_val = float_val;
    type = VT_FLOAT;
}


void LVOV::reset()
{
    type = VT_ANY;
    variable_name = "";
    str_val = "";
    int_val = 0;
    float_val = 0.0;
}


string LVOV::to_string()
{
    if(type == VT_ANY)
        return "";

    if(type == VT_STR)
        return str_val;

    if(type == VT_INT)
        return std::to_string(int_val);

    if(type == VT_FLOAT)
        return std::to_string(float_val);

    if(type == VT_REF)
        return variable_name;

    throw std::invalid_argument("unknown ValueType");
}


bool LVOV::from_string(Glib::ustring input)
{
    if(type == VT_STR)
    {
        str_val = input;
        return true;
    }

    else if(type == VT_REF)
    {
        variable_name = input;
        return true;
    }

    else if(type == VT_FLOAT)
        return string_to_float(float_val, input);
        
    else if(type == VT_INT)
        return string_to_int(int_val, input); 
   
    else
        throw std::invalid_argument("unknown ValueType");
}


LVOV::operator int() const
{
    return int_val;
}


LVOV::operator string() const
{
    return type == VT_STR ? str_val : variable_name;
}


LVOV::operator float() const
{
    return float_val;
}



// Value Input Controller


ValueInputController::ValueInputController(Gtk::Container *parent_container, string name)
{
    vector<ValueType> allowed_types(std::begin(AllValueTypes), std::end(AllValueTypes));
    _init(parent_container, allowed_types, name);   
}


ValueInputController::ValueInputController(Gtk::Container *parent_container, vector<ValueType> allowed_types, string name)
{   
    _init(parent_container, allowed_types, name);
}


void ValueInputController::_init(Gtk::Container *parent_container, vector<ValueType> allowed_types, string name)
{
    is_updating = false;

    // Prepare widgets
    title_label.set_markup("<b>" + name + "</b>");
    value_type_label.set_text("value type");
    value_label.set_text("value");
    
    container_grid.attach(title_label, 0, 0, 2, 1);
    container_grid.attach(value_type_label, 0, 1, 1, 1);
    container_grid.attach(value_type_combobox, 1, 1, 1, 1);
    container_grid.attach(value_label, 0, 2, 1, 1);
    container_grid.attach(value_entry, 1, 2, 1, 1);

    int i;
    for(i=0; i<allowed_types.size(); i++)
    {
        ValueType vt = allowed_types[i];
        string str = value_type_to_string(vt);
        value_type_combobox.append(str);
    }

    if(i == 0)
    {
        value_type_combobox.set_sensitive(false);
        value_type_label.set_sensitive(false);
    }

    parent_container->add(container_grid);
    
    title_label.show();
    value_type_label.show();
    value_label.show();

    value_type_combobox.show();
    value_entry.show();

    container_grid.show();

    // Set widgets to default value
    set_value(value);
}


void ValueInputController::on_value_type_changed()
{
    ValueType type = string_to_value_type(value_type_combobox.get_active_text());
    value.type = type;
    set_value(value);
}


void ValueInputController::on_value_changed()
{
    auto text = value_entry.get_text();
    if(value.from_string(text))
        return;

    set_value(value); 
}


void ValueInputController::set_value(LVOV new_value)
{
    is_updating = true;
    value = new_value;

    string typ_str = value_type_to_string(value.type);
    value_type_combobox.set_active_text(typ_str);

    value_entry.set_text(value.to_string());

    is_updating = false;
}


