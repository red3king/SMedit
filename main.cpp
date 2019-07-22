#include <iostream>
#include <memory>
#include <gtkmm.h>
#include <map>

#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "log.h"
#include "historymanager/historymanager.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/state_ops.h"
#include "historymanager/operations/resource_ops.h"
#include "historymanager/operations/resourcelock_ops.h"
#include "historymanager/operations/transition_ops.h"
#include "gui/gui_context.h"
#include "signals.h"
#include "cursor.h"


class MainWindow : public Gtk::ApplicationWindow 
{
    public:
    
        MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , 
            builder{builder}
        {
            set_title("smedit2");
                
            Gtk::Button* closeBtn = nullptr;
            builder->get_widget("closebutton", closeBtn);
            closeBtn->signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_close_click));
            
            // GLArea stuff
            gl_area = nullptr;
            builder->get_widget("gl_area", gl_area);

            history_manager = new HistoryManager(50, 20, 15);
            
            auto cr8 = OpMachineCreate();
            history_manager->submit_operation(cr8);
            Machine* machine = history_manager->current_project.machines[0];

            gui_context = new GUIContext(gl_area, history_manager);            
            gui_context->set_machine(machine);

            auto stcr8 = OpStateCreate(machine, 20, 20);
            history_manager->submit_operation(stcr8);
            machine->states[0]->name = "NEW STATE!!";
            auto stcr82 = OpStateCreate(machine, 220, 30);
            history_manager->submit_operation(stcr82);
            
            auto resource_create = OpResourceCreate("resource #1", "ftp://www.exe");
            unsigned int res_id = history_manager->submit_operation(resource_create);
            Resource* res = history_manager->current_project.get_resource_by_id(res_id);
            auto resourcelock_create = OpResourceLockCreate(machine, res, 20, 222);
            history_manager->submit_operation(resourcelock_create);            

            auto transition_create = OpTransitionCreate(machine, 400, 400, 600, 420);
            history_manager->submit_operation(transition_create);

            gl_area->signal_realize().connect(sigc::mem_fun(this, &MainWindow::connect_cursor_signals));
        }

        virtual ~MainWindow() = default;

        HistoryManager* history_manager;

        Gtk::GLArea* gl_area;
        GUIContext* gui_context;
        
        void on_close_click()
        {
            log("closing.");
            hide();
        }

        void connect_cursor_signals()
        {
            main_gdk_win = gl_area->get_window();
            register_set_cursor_handler();
        }

    private:
        Glib::RefPtr<Gtk::Builder> builder;
};



int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "de.engelmarkus.example");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout_gl.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("mainwin", wnd);
    
    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

