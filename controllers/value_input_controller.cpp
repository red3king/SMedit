#include <stdexcept>

#include "value_input_controller.h"
#include "utils.h"


ValueInputController::ValueInputController()
{
    vector<ValueType> allowed_types(std::begin(AllValueTypes), std::end(AllValueTypes));
    _init(allowed_types, false, "");
}


ValueInputController::ValueInputController(string name)
{
    vector<ValueType> allowed_types(std::begin(AllValueTypes), std::end(AllValueTypes));
    _init(allowed_types, true, name);   
}


ValueInputController::ValueInputController(vector<ValueType> allowed_types, string name)
{   
    _init(allowed_types, true, name);
}


void ValueInputController::_init(vector<ValueType> allowed_types, bool has_name, string name)
{
    this->has_name = has_name;
    is_updating = false;

    // Prepare widgets
    if(has_name)
    {
        left_align(&title_label);
        title_label.set_markup("<b>" + name + "</b>");
    }

    value_type_label.set_text("Type:");
    value_label.set_text("Value:");
    
    set_margins(&value_type_label, 4);
    left_align(&value_type_label);
    set_margins(&value_label, 4);
    left_align(&value_label);

    set_margins(&title_label, 4);

    set_margins(&value_type_combobox, 4);
    right_align(&value_type_combobox);
    
    set_margins(&value_entry, 4);
    right_align(&value_entry);

    int noffset = has_name ? 0 : 1;

    if(has_name)
        container_grid.attach(title_label, 0, 0, 2, 1);

    container_grid.attach(value_type_label, 0, 1 + noffset, 1, 1);
    container_grid.attach(value_type_combobox, 1, 1 + noffset, 1, 1);
    container_grid.attach(value_label, 0, 2 + noffset, 1, 1);
    container_grid.attach(value_entry, 1, 2 + noffset, 1, 1);

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

    // Set widgets to default value
    set_value(value);
    
    value_type_combobox.signal_changed().connect(sigc::mem_fun(this, &ValueInputController::on_value_type_changed));
    value_entry.signal_changed().connect(sigc::mem_fun(this, &ValueInputController::on_value_changed));
}


void ValueInputController::attach(Gtk::Container* parent_container)
{
    parent_container->add(container_grid);

    if(has_name)
        title_label.show();
    
    value_type_label.show();
    value_label.show();

    value_type_combobox.show();
    value_entry.show();

    container_grid.show();
}



void ValueInputController::on_value_type_changed()
{
    if(is_updating)
        return;

    ValueType type = string_to_value_type(value_type_combobox.get_active_text());
    value.type = type;
    set_value(value);
    value_changed_signal.emit(value);
}


void ValueInputController::on_value_changed()
{
    if(is_updating)
        return;

    auto text = value_entry.get_text();
    if(!value.from_string(text))
        return;

    set_value(value); 
    value_changed_signal.emit(value);
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


void ValueInputController::set_enabled(bool enabled)
{
    value_entry.set_sensitive(enabled);
    value_type_combobox.set_sensitive(enabled);
}
