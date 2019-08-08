#pragma once

#include <iostream>
#include <memory>
#include <gtkmm.h>
#include <map>

#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "controllers/resources_controller.h"
#include "historymanager/historymanager.h"
#include "gui/gui_context.h"
#include "signals.h"
#include "cursor.h"


class MainWindow : public Gtk::ApplicationWindow 
{
    public:
    
        MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder);
        virtual ~MainWindow() = default;

        HistoryManager* history_manager;
        GUIContext* gui_context;

        ResourcesController* resources_controller;

        // UI state variables
        bool project_open;
        bool machine_selected;

        // Widgets
        Gtk::GLArea* machine_edit_gl_area;
        
        Gtk::ImageMenuItem *file_new, *file_open, *file_save, *file_save_as, *file_quit, 
            *edit_undo, *edit_redo, *edit_cut, *edit_copy, *edit_paste, *edit_delete;

        // init functions
        void get_widgets();
        void prepare_signals();
        void connect_cursor_signals();

        void temp_create_operations();
        
        // signal handlers
        void on_project_open();
        void on_project_close();
        void on_machine_selected(Machine* machine);
        void on_close_click();
        void on_new_click();

        void _update_enabled();

    private:
        Glib::RefPtr<Gtk::Builder> builder;
};

