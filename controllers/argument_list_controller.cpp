#include "argument_list_controller.h"
#include "utils.h"


ArgumentListController::ArgumentListController()
{
    is_emitting = false;
    is_setting = false;
    selected_index = -1;

    // prep widgets
    title_label.set_markup("<b>Task Args:</b>");
    set_margins(&title_label, 4);
    left_align(&title_label);

    create_button.set_label("Create");
    delete_button.set_label("Delete");

    arg_name_label.set_text("Name:");
    set_margins(&arg_name_label, 4);
    left_align(&arg_name_label);

    set_margins(&arg_name_entry, 4);
    right_align(&arg_name_entry);

    set_margins(&create_button, 4);
    set_margins(&delete_button, 4);
    center_align(&create_button);
    center_align(&delete_button);

    value_input_controller = new ValueInputController();
    list_view_controller = new ListViewController(&tree_view);

    tree_view.property_height_request() = 150;
    set_margins(&tree_view, 4);

    // attach widgets
    value_input_controller->attach(&value_input_box);

    main_grid.attach(title_label, 0, 0, 2, 1);
    main_grid.attach(tree_view, 0, 1, 2, 1);
    main_grid.attach(arg_name_label, 0, 2, 1, 1);
    main_grid.attach(arg_name_entry, 1, 2, 1, 1);
    main_grid.attach(value_input_box, 0, 3, 2, 2);
    main_grid.attach(create_button, 0, 5, 1, 1);
    main_grid.attach(delete_button, 1, 5, 1, 1);

    // signals
    create_button.signal_clicked().connect(sigc::mem_fun(this, &ArgumentListController::on_create_clicked));
    delete_button.signal_clicked().connect(sigc::mem_fun(this, &ArgumentListController::on_delete_clicked));
    arg_name_entry.signal_changed().connect(sigc::mem_fun(this, &ArgumentListController::on_name_changed));
    value_input_controller->value_changed_signal.connect(sigc::mem_fun(this, &ArgumentListController::on_value_changed));
    list_view_controller->selection_changed_signal.connect(sigc::mem_fun(this, &ArgumentListController::on_selection_changed));
}


void ArgumentListController::attach(Gtk::Container* parent_container)
{
    parent_container->add(main_grid);
    title_label.show();
    main_grid.show();
    tree_view.show();
    arg_name_label.show();
    arg_name_entry.show();
    value_input_box.show();
    create_button.show();
    delete_button.show();
}


void ArgumentListController::_update()
{
    bool item_selected = selected_index != -1;
    arg_name_entry.set_sensitive(item_selected);
    delete_button.set_sensitive(item_selected);
    value_input_controller->set_enabled(item_selected);    
}


void ArgumentListController::_fire_signal()
{
    is_emitting = true;
    on_args_changed.emit(arg_list);
    is_emitting = false;
}


void ArgumentListController::set_args(vector<Arg> arg_list)
{
    if(is_emitting)
        return;

    is_setting = true;

    this->arg_list = arg_list;
    selected_index = -1;
    list_view_controller->clear();
    
    for(unsigned int i=0; i<arg_list.size(); i++)
    {
        Arg arg = arg_list[i];
        list_view_controller->add_item(arg.describe(), i);
    }
    
    is_setting = false;

    _update_selected();
}


void ArgumentListController::_update_selected()
{
    is_setting = true;

    if(selected_index == -1)
    {
        value_input_controller->set_value(LVOV(""));
        arg_name_entry.set_text("");
    }

    else
    {
        value_input_controller->set_value(arg_list[selected_index].argument_value);
        arg_name_entry.set_text(arg_list[selected_index].variable_name);
    }

    is_setting = false;
    _update();
}


void ArgumentListController::on_create_clicked()
{
    Arg new_arg("argument", LVOV(""));
    
    unsigned int i = arg_list.size();
    arg_list.push_back(new_arg);
    list_view_controller->add_item(new_arg.describe(), i);
    selected_index = i;

    is_setting = true;
    list_view_controller->select_item(i);
    is_setting = false;

    _update_selected();
    _fire_signal();
}


void ArgumentListController::on_delete_clicked()
{
    arg_list.erase(arg_list.begin() + selected_index);
    set_args(arg_list);
    _fire_signal();
}


void ArgumentListController::on_name_changed()
{
    if(is_setting)
        return;

    arg_list[selected_index].variable_name = arg_name_entry.get_text();
    list_view_controller->change_item(arg_list[selected_index].describe(), (unsigned int) selected_index);
    _fire_signal();
}


void ArgumentListController::on_value_changed(LVOV value)
{
    if(is_setting)
        return;

    arg_list[selected_index].argument_value = value;
    list_view_controller->change_item(arg_list[selected_index].describe(), (unsigned int) selected_index);
    _fire_signal();
}


void ArgumentListController::on_selection_changed(unsigned int index)
{
    if(is_setting)
        return;

    selected_index = index;
    _update_selected();
}


