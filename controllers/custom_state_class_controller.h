#pragma once

#include "top_controller.h"
#include "list_view_controller.h"
#include "models/entity.h"
#include "signals.h"
#include "historymanager/historymanager.h"
#include "saveload.h"


class CustomStateClassController : public TopController
{
    // Custom state classes tab
    public:
        CustomStateClassController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder, Gtk::Window* main_window);
        
    private:
        Gtk::TreeView* csc_tree_view;
        Gtk::Button* create_button;
        Gtk::Button* delete_button;
        Gtk::Button* reload_button;
        Gtk::Label* name_label;
        Gtk::Label* path_label;
        Gtk::Label* preview_label;
        Gtk::Entry* name_entry;
        Gtk::FileChooserButton* path_file_chooser;

        Gtk::Window* main_window;

        ProjectInfo* current_project_info;
        ListViewController* list_view_controller;

        // ui state vars
        int num_items;
        CustomStateClass* selected_csc;
        unsigned int selected_csc_id;

        bool is_setting;

        void reset(bool reload);
        void load_from(Project& current_project, bool reload);

        void on_project_path_set();
        
        void on_focus_operation(Operation* operation, unsigned int result);
        void on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type);
        void on_selection_changed(unsigned int entity_id);

        void on_name_changed();
        void on_path_changed();

        void on_create_clicked();
        void on_delete_clicked();
        void on_reload_clicked();

        void _build_preview();
        void _rebuild_treeview(unsigned int deleted_id);
        void _update_enabled();
};
