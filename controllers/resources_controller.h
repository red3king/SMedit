#pragma once

#include "top_controller.h"
#include "list_view_controller.h"
#include "models/entity.h"
#include "signals.h"
#include "historymanager/historymanager.h"


class ResourcesController : public TopController
{
    // Handles the resources tab
    public:
        ResourcesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder, Gtk::Window* main_window);
        
    private:
        Gtk::TreeView* resource_tree_view;
        Gtk::Button *create_button, *delete_button;
        Gtk::Label *name_label, *path_label;
        Gtk::Entry* name_entry;
        Gtk::FileChooserButton* path_file_chooser;

        Gtk::Window* main_window;

        ListViewController* list_view_controller;

        // ui state vars
        int num_items;
        Resource* selected_resource;
        unsigned int selected_resource_id;

        bool is_setting;

        void reset(bool reload);
        void load_from(Project& current_project, bool reload);

        void on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);
        void on_selection_changed(unsigned int entity_id);

        void on_name_changed();
        void on_path_changed();

        void on_create_clicked();
        void on_delete_clicked();

        void _rebuild_treeview(unsigned int deleted_id);
        void _update_enabled();
};
