#include "selected_state.h"
#include "historymanager/operations/state_ops.h"


#define STK_INITIAL_ID "state_initial_selected"
#define STK_CODE_ID "state_code_selected"
#define STK_RETURN_ID "state_return_selected"
#define STK_SPAWN_ID "state_spawn_selected"
#define STK_JOIN_ID "state_join_selected"


SelectedState::SelectedState(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : SelectedItemController(history_manager)
{
    selected_state = nullptr;

    builder->get_widget("delete_state_button", delete_button);
    builder->get_widget("selected_state_title", title_label);
    builder->get_widget("state_name_entry", name_entry);
    builder->get_widget("state_type_combobox", type_combobox);    
    builder->get_widget("selected_state_stack", state_types_stack);

    builder->get_widget("join_pidvar_entry", join_pidvar_entry);
    
    Gtk::Box* return_grid;
    builder->get_widget("state_return_selected", return_grid);
    return_value_input = new ValueInputController("return value");
    return_value_input->attach(return_grid);
    return_value_input->value_changed_signal.connect(sigc::mem_fun(this, &SelectedState::on_retval_changed));

    init_initial_state_ctrl(builder);

    // init code state ctrl
    Gtk::ScrolledWindow* code_box;
    builder->get_widget("state_code_scrolled", code_box);
    code_state_ctrl = new CodeStateController(code_box);
    code_state_ctrl->signal_code_changed.connect(sigc::mem_fun(this, &SelectedState::on_code_changed));

    // init spawn state ctrl
    Gtk::Box* spawn_box;
    builder->get_widget("state_spawn_selected", spawn_box);
    spawn_state_ctrl = new SpawnStateController(history_manager);
    spawn_state_ctrl->attach(spawn_box);


    name_entry->signal_changed().connect(sigc::mem_fun(this, &SelectedState::on_name_changed));
    type_combobox->signal_changed().connect(sigc::mem_fun(this, &SelectedState::on_type_changed));
    delete_button->signal_clicked().connect(sigc::mem_fun(this, &SelectedState::on_delete_clicked));
    join_pidvar_entry->signal_changed().connect(sigc::mem_fun(this, &SelectedState::on_jpidvar_changed));
}


void SelectedState::init_initial_state_ctrl(Glib::RefPtr<Gtk::Builder> const& builder)
{
    Gtk::TreeView* tree_view;
    Gtk::Button *delete_button, *create_button;
    Gtk::Entry* arg_name_entry;
    Gtk::ComboBoxText* arg_type_cb;

    builder->get_widget("initial_state_arg_treeview", tree_view);
    builder->get_widget("initial_state_delete_button", delete_button);
    builder->get_widget("initial_state_create_button", create_button);
    builder->get_widget("initial_state_name_entry", arg_name_entry);
    builder->get_widget("initial_state_type_combobox", arg_type_cb);

    initial_state_ctrl = new InitialStateController(tree_view, delete_button, create_button, 
            arg_name_entry, arg_type_cb);
    initial_state_ctrl->config_changed_signal.connect(sigc::mem_fun(this, &SelectedState::on_initial_state_changed));
}


void SelectedState::on_code_changed(string code)
{
    auto op = OpStateCode(owning_machine, selected_state, code);
    history_manager->submit_operation(op);
}


void SelectedState::on_initial_state_changed(vector<ArgDef> arguments)
{
    auto op = OpStateInitialCfg(owning_machine, selected_state, arguments);
    history_manager->submit_operation(op);
}


void SelectedState::on_retval_changed(LVOV lvov)
{
    auto op = OpStateRetVal(owning_machine, selected_state, lvov);
    history_manager->submit_operation(op);
}


void SelectedState::on_jpidvar_changed()
{
    auto op = OpStateJoinPidVar(owning_machine, selected_state, join_pidvar_entry->get_text());
    history_manager->submit_operation(op);
}


void SelectedState::on_name_changed()
{
    auto op = OpStateName(owning_machine, selected_state, name_entry->get_text());
    history_manager->submit_operation(op);
}


void SelectedState::on_type_changed()
{
    StateType type = string_to_state_type(type_combobox->get_active_text());
    auto op = OpStateType(owning_machine, selected_state, type);
    history_manager->submit_operation(op);
}


void SelectedState::on_delete_clicked()
{
    auto op = OpStateDelete(owning_machine, selected_state);
    history_manager->submit_operation(op);
}


void SelectedState::set_selected_impl(Machine* owning_machine, Entity* entity)
{
    selected_state = dynamic_cast<State*>(entity);
}


Entity* SelectedState::get_entity()
{
    return (Entity*)selected_state;
}


void SelectedState::update()
{
    if(selected_state == nullptr)
        return;

    title_label->set_text("State " + std::to_string(selected_state->id));
    name_entry->set_text(selected_state->name);
    type_combobox->set_active_text(state_type_to_string(selected_state->type));

    join_pidvar_entry->set_text(selected_state->join_pid_variable);
    return_value_input->set_value(selected_state->return_value);
    initial_state_ctrl->set_config(selected_state->initial_args);
    code_state_ctrl->set_text(selected_state->code);
    spawn_state_ctrl->set_state(owning_machine, selected_state);

    if(selected_state->type == INITIAL)
        state_types_stack->set_visible_child(STK_INITIAL_ID);
    else if(selected_state->type == CODE)
        state_types_stack->set_visible_child(STK_CODE_ID);
    else if(selected_state->type == RETURN)
        state_types_stack->set_visible_child(STK_RETURN_ID);
    else if(selected_state->type == JOIN)
        state_types_stack->set_visible_child(STK_JOIN_ID);
    else if(selected_state->type == SPAWN)
        state_types_stack->set_visible_child(STK_SPAWN_ID);
}