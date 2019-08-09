#include "main.h"

#include "log.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/state_ops.h"
#include "historymanager/operations/resource_ops.h"
#include "historymanager/operations/resourcelock_ops.h"
#include "historymanager/operations/transition_ops.h"


MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , builder{builder}
{
    set_title("state machine editor");

    project_open = false;
    machine_selected = false;

    get_widgets();
    prepare_signals();

    history_manager = new HistoryManager(50, 20, 15);
    gui_context = new GUIContext(machine_edit_gl_area, history_manager);            
    
    resources_controller = new ResourcesController(history_manager, builder);
    machines_controller = new MachinesController(history_manager, builder, &(gui_context->gui_state.draw_context));

    machine_edit_gl_area->signal_realize().connect(sigc::mem_fun(this, &MainWindow::connect_cursor_signals));
}


void MainWindow::get_widgets()
{
    // Get widgets
    builder->get_widget("machine_edit_gl_area", machine_edit_gl_area);

    // Menu items
    builder->get_widget("file_new", file_new);    
    builder->get_widget("file_open", file_open);
    builder->get_widget("file_save", file_save);
    builder->get_widget("file_save_as", file_save_as);
    builder->get_widget("file_quit", file_quit);

    builder->get_widget("edit_undo", edit_undo);
    builder->get_widget("edit_redo", edit_redo);
    builder->get_widget("edit_cut", edit_cut);
    builder->get_widget("edit_copy", edit_copy);
    builder->get_widget("edit_paste", edit_paste);
    builder->get_widget("edit_delete", edit_delete);

}


void MainWindow::prepare_signals()
{
    // smedit signals
    signals.project_close.connect(sigc::mem_fun(this, &MainWindow::on_project_close));
    signals.project_open.connect(sigc::mem_fun(this, &MainWindow::on_project_open));
    signals.machine_selected.connect(sigc::mem_fun(this, &MainWindow::on_machine_selected));

    // click signals
    file_quit->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_close_click));
    file_new->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_new_click));
}


void MainWindow::temp_create_operations()
{
    auto cr8 = OpMachineCreate("machine one");
    history_manager->submit_operation(cr8);
    Machine* machine = history_manager->current_project.machines[0];

    signals.machine_selected.emit(machine);

    auto stcr8 = OpStateCreate(machine, 20, 20);
    history_manager->submit_operation(stcr8);
    machine->states[0]->name = "NEW STATE!!";
    auto stcr82 = OpStateCreate(machine, 220, 30);
    history_manager->submit_operation(stcr82);

    auto resource_create0 = OpResourceCreate("resource #0", "omg");
    history_manager->submit_operation(resource_create0);

    auto resource_create = OpResourceCreate("resource #1", "ftp://www.exe");
    unsigned int res_id = history_manager->submit_operation(resource_create);
    Resource* res = history_manager->current_project.get_resource_by_id(res_id);
    auto resourcelock_create = OpResourceLockCreate(machine, res, 20, 222);
    history_manager->submit_operation(resourcelock_create);            

    auto transition_create = OpTransitionCreate(machine, 400, 400, 600, 420);
    history_manager->submit_operation(transition_create);

    auto transition = machine->transitions[0];
    auto tenco = OpTransitionEvName(machine, transition, "on death");
    history_manager->submit_operation(tenco);
}


void MainWindow::on_project_open()
{
    project_open = true;
    _update_enabled();
}


void MainWindow::on_project_close()
{
    project_open = false;
    _update_enabled();
}


void MainWindow::on_machine_selected(Machine* machine)
{

    if(machine == nullptr)
    {
        gui_context->unset_machine();
        machine_selected = false;
    }
    else
    {
        gui_context->set_machine(machine);
        machine_selected = true; 
    }

    _update_enabled();
}


void MainWindow::on_close_click()
{
    log("closing.");
    hide();
}


void MainWindow::on_new_click()
{
    bool unsaved_changes = true; // TODO
    if(unsaved_changes)
    {
        Gtk::MessageDialog dialog(*this, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("You have unsaved changes, do you want to create a new file?");

        int result = dialog.run();
        switch(result)
        {
            case Gtk::RESPONSE_OK:
                break;
            default:
                return;
        }
    }

    history_manager->new_project();
    signals.project_open();
    signals.machine_selected(nullptr);
    
    temp_create_operations();
}


void MainWindow::connect_cursor_signals()
{
    main_gdk_win = machine_edit_gl_area->get_window();
    register_set_cursor_handler();
}


void MainWindow::_update_enabled()
{
    bool save_enabled = project_open;
    file_save->set_sensitive(save_enabled);
    file_save_as->set_sensitive(save_enabled);
    
    edit_undo->set_sensitive(false);
    edit_redo->set_sensitive(false);
    edit_cut->set_sensitive(false);
    edit_copy->set_sensitive(false);
    edit_paste->set_sensitive(false);
    edit_delete->set_sensitive(false);
}


int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "TODO.setup.domain");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout_new.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("main_window", wnd);

    signals.project_close.emit();
//    signals.machine_selected.emit(nullptr);

    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

