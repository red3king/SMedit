#include "initial_state_ctrl.h"
#include "utils.h"


InitialStateController::InitialStateController(Gtk::TreeView* tree_view, Gtk::Button* delete_button, Gtk::Button* create_button, Gtk::Entry* arg_name_entry, Gtk::ComboBoxText* arg_type_combobox)
{
    is_updating = false;
    is_emitting = false;
    selected_row = -1;

    this->arg_name_entry = arg_name_entry;
    this->arg_type_combobox = arg_type_combobox;
    this->delete_button = delete_button;

    prepare_value_type_combobox(arg_type_combobox);

    list_view_controller = new ListViewController(tree_view, "Machine launch args:");
    list_view_controller->selection_changed_signal.connect(sigc::mem_fun(this, &InitialStateController::on_listview_selection_changed));

    arg_name_entry->signal_changed().connect(sigc::mem_fun(this, &InitialStateController::on_arg_name_changed));
    arg_type_combobox->signal_changed().connect(sigc::mem_fun(this, &InitialStateController::on_arg_type_changed));

    delete_button->signal_clicked().connect(sigc::mem_fun(this, &InitialStateController::on_delete_clicked));
    create_button->signal_clicked().connect(sigc::mem_fun(this, &InitialStateController::on_create_clicked));

    _update();
}


void InitialStateController::_update()
{
    bool inputs_enabled = arguments.size() > 0 && selected_row > -1;
    arg_name_entry->set_sensitive(inputs_enabled);
    arg_type_combobox->set_sensitive(inputs_enabled);
    delete_button->set_sensitive(inputs_enabled);
}


vector<ArgDef> InitialStateController::get_config()
{
    return arguments;
}


void InitialStateController::set_config(vector<ArgDef> arguments)
{
    if(is_emitting)
        return;

    this->arguments = arguments;
    list_view_controller->clear();
    selected_row = -1;

    for(unsigned int i=0; i<arguments.size(); i++)
        list_view_controller->add_item(arguments[i].describe(), i);
    
    _update();    
}


void InitialStateController::on_listview_selection_changed(unsigned int selection)
{
    selected_row = selection;
    ArgDef def = arguments[selection];
    is_updating = true;
    arg_name_entry->set_text(def.variable_name);
    arg_type_combobox->set_active_text(value_type_to_string(def.value_type));
    is_updating = false;
    _update();
}


void InitialStateController::on_create_clicked()
{
    arguments.push_back(ArgDef("new argument", VT_ANY));
    unsigned int i = arguments.size() - 1;
    list_view_controller->add_item(arguments[i].describe(), i);
    list_view_controller->select_item(i);
    _fire_event();
}


void InitialStateController::on_delete_clicked()
{ 
    arguments.erase(arguments.begin() + selected_row);
    set_config(arguments);
    _update();
    _fire_event();
}


void InitialStateController::on_arg_name_changed()
{
    if(is_updating)
        return;

    auto name = arg_name_entry->get_text();
    arguments[selected_row].variable_name = name;
    update_gridview(selected_row);
    _fire_event();
}


void InitialStateController::on_arg_type_changed()
{
    if(is_updating)
        return;

    ValueType vt = string_to_value_type(arg_type_combobox->get_active_text());
    arguments[selected_row].value_type = vt;
    update_gridview(selected_row);
    _fire_event();
}


void InitialStateController::update_gridview(unsigned int row)
{
    string value = arguments[row].describe();
    list_view_controller->change_item(value, row);    
}


void InitialStateController::_fire_event()
{
    is_emitting = true;
    config_changed_signal.emit(arguments);
    is_emitting = false;
}
