#include "main.h"

#include "log.h"
#include "utils.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/state_ops.h"
#include "historymanager/operations/resource_ops.h"
#include "historymanager/operations/resourcelock_ops.h"
#include "historymanager/operations/transition_ops.h"


MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , builder{builder}
{

    current_filename = "";
    project_open = false;
    may_undo = false;
    may_redo = false;
    unsaved_changes = false;

    get_widgets();

    history_manager = new HistoryManager(50, 20, 15);

    resources_controller = new ResourcesController(history_manager, builder, this);
    machines_controller = new MachinesController(history_manager, builder, this);
    run_controller = new RunController(history_manager, &project_info, builder);

    prepare_signals();
    machine_edit_gl_area->signal_realize().connect(sigc::mem_fun(this, &MainWindow::connect_cursor_signals));
    _regen_title();
}


void MainWindow::_regen_title()
{
    string project_title = current_filename == "" ? "unsaved" : current_filename;
    if(unsaved_changes)
        project_title += "*";

    set_title("smedit - " + project_title);
}


void MainWindow::get_widgets()
{
    // Get widgets
    builder->get_widget("machine_edit_gl_area", machine_edit_gl_area);
    builder->get_widget("main_stack", main_stack);
    builder->get_widget("main_switcher", main_switcher);

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

    builder->get_widget("about_licenses", about_licenses);
}


void MainWindow::prepare_signals()
{
    // smedit signals
    signals.project_close.connect(sigc::mem_fun(this, &MainWindow::on_project_close));
    signals.project_open.connect(sigc::mem_fun(this, &MainWindow::on_project_open));

    history_manager->signal_changed.connect(sigc::mem_fun(this, &MainWindow::on_history_changed)); 
    history_manager->signal_unsaved_changes.connect(sigc::mem_fun(this, &MainWindow::on_unsaved_changes_changed));

    // click signals
    file_quit->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_close_click));
    file_new->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_new_click));
    file_save->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_save_click));
    file_save_as->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_save_as_click));
    file_open->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_open_click));
    edit_undo->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_undo_click));
    edit_redo->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_redo_click));    
    about_licenses->signal_activate().connect(sigc::mem_fun(this, &MainWindow::on_about_click));
}


void MainWindow::on_history_changed(bool may_undo, bool may_redo)
{
    this->may_undo = may_undo;
    this->may_redo = may_redo;
    _update_enabled();    
}


void MainWindow::on_unsaved_changes_changed(bool unsaved_changes)
{
    this->unsaved_changes = unsaved_changes;
    _regen_title();
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


void MainWindow::on_close_click()
{
    if(unsaved_changes)
    {
        Gtk::MessageDialog dialog(*this, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("You have unsaved changes, do you still want to quit?");

        if(dialog.run() != Gtk::RESPONSE_OK)
            return;
    }

    log("closing.");
    hide();
}


void MainWindow::on_undo_click()
{
    history_manager->undo();
}


void MainWindow::on_redo_click()
{
    history_manager->redo();
}


void MainWindow::on_new_click()
{
    if(unsaved_changes)
    {
        Gtk::MessageDialog dialog(*this, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("You have unsaved changes, do you still want to create a new project?");

        if(dialog.run() != Gtk::RESPONSE_OK)
            return;
    }

    current_filename = "";
    history_manager->new_project();
    _regen_title();
}


void MainWindow::on_save_click()
{
   _do_save(); 
}


void MainWindow::on_open_click()
{
    if(unsaved_changes)
    {
        Gtk::MessageDialog dialog(*this, "Hey", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
        dialog.set_secondary_text("You have unsaved changes, do you still want to open a project?");

        if(dialog.run() != Gtk::RESPONSE_OK)
            return;
    }

    Gtk::FileChooserDialog chooser(
            "Choose a folder to open.", 
            Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    chooser.set_transient_for(*this);
    chooser.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    chooser.add_button("Open", Gtk::RESPONSE_OK);

    if(chooser.run() != Gtk::RESPONSE_OK)
        return;

    Project proj;
    string filename = chooser.get_filename();
    IOResult result = load_project(proj, project_info, filename);

    if(!result.success)
    {
        display_error(*this, result.fail_msg);
        return;
    }
    
    current_filename = filename;
    history_manager->load_project(proj);
    signals.fire_project_loaded();
    _regen_title();
}


void MainWindow::on_save_as_click()
{
    Gtk::FileChooserDialog chooser(
            "Choose a folder to save to.", 
            Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    chooser.set_transient_for(*this);
    chooser.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    chooser.add_button("Save", Gtk::RESPONSE_OK);

    if(chooser.run() != Gtk::RESPONSE_OK)
        return;
    
    current_filename = chooser.get_filename();
    _do_save();
    _regen_title();
}


void MainWindow::_do_save()
{
    IOResult result = save_project(history_manager->current_project, project_info, current_filename);
    signals.fire_project_saved();

    if(!result.success)
    {
        display_error(*this, result.fail_msg);
        return;
    }

    history_manager->set_saved();
}


void MainWindow::on_about_click()
{
    Gtk::MessageDialog dialog(*this, "About", false, Gtk::MESSAGE_OTHER, Gtk::BUTTONS_CLOSE);
    Gtk::Box* dialog_box = dialog.get_message_area();
    Gtk::Box version_box(Gtk::ORIENTATION_HORIZONTAL);
    Gtk::Label version_version_label, version_title_label, licenses_label;
    Gtk::ScrolledWindow scrolled_area;
    Gtk::TextView licenses_area;

    version_title_label.set_markup("<b>Version:</b>");
    version_version_label.set_text("0.0.1");
    version_box.pack_start(version_title_label, false, false, 0);
    version_box.pack_start(version_version_label, false, false, 0);
    
    set_margins(&version_title_label, 4);
    set_margins(&version_version_label, 4);
    left_align(&version_title_label);
    right_align(&version_version_label);

    dialog_box->pack_start(version_box, false, false, 0);

    licenses_label.set_markup("<b>Licenses:</b>");
    left_align(&licenses_label);
    set_margins(&licenses_label, 4);
    
    dialog_box->pack_start(licenses_label, false, false, 0);

    scrolled_area.add(licenses_area);
    auto buf = Gtk::TextBuffer::create();
    licenses_area.set_buffer(buf);
    licenses_area.set_editable(false);

    string license_txt = "failed to read licenses.txt";
    file_to_string("licenses.txt", license_txt);

    buf->set_text(license_txt);
    scrolled_area.property_height_request() = 200;

    dialog_box->pack_start(scrolled_area, false, false, 0);
    dialog_box->property_width_request() = 400;

    version_version_label.show();
    version_title_label.show();
    licenses_label.show();
    version_box.show();
    scrolled_area.show();
    licenses_area.show();

    dialog.run();
}


void MainWindow::connect_cursor_signals()
{
    main_gdk_win = machine_edit_gl_area->get_window();
    register_set_cursor_handler();
}


void MainWindow::_update_enabled()
{
    bool save_as_enabled = project_open;
    bool save_enabled = project_open && current_filename != "";
    file_save->set_sensitive(save_enabled);
    file_save_as->set_sensitive(save_as_enabled);
    
    edit_undo->set_sensitive(project_open && may_undo);
    edit_redo->set_sensitive(project_open && may_redo);

    edit_cut->set_sensitive(false);
    edit_copy->set_sensitive(false);
    edit_paste->set_sensitive(false);
    edit_delete->set_sensitive(false);

    main_stack->set_sensitive(project_open);
    main_switcher->set_sensitive(project_open);
}


int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "TODO.setup.domain");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout_new.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("main_window", wnd);

    signals.project_close.emit();

    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

