#pragma once

#include "list_view_controller.h"
#include "selected_item/selected_resourcelock.h"
#include "historymanager/historymanager.h"
#include "models/machine.h"
#include "gui/draw_context.h"
#include "signals.h"


class MachinesController
{
    public:
        MachinesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

    private:
        ListViewController* list_view_controller;
        SelectedResourceLock* selected_resourcelock_ctrl;

        HistoryManager* history_manager;
        DrawContext* draw_context;
        GUIContext* gui_context;

        // ui state
        bool project_open;
        Machine* selected_machine;
        
        bool disable_input_signals;

        // widgets
        Gtk::TreeView* tree_view;
        Gtk::Button *create_button, *delete_button;
        Gtk::Button *create_transition_button, *create_state_button, *create_resourcelock_button;
        Gtk::Label *name_label, *launch_on_start_label;
        Gtk::Entry* name_entry;
        Gtk::Switch* launch_on_start_switch;
        Gtk::Stack* selected_item_stack;
        Gtk::GLArea* gl_area;

        void on_model_selected(Machine* machine, EntityType entity_type, Entity* entity);
        void on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);
        void on_selection_changed(unsigned int machine_id);
        void on_project_open();
        void on_project_close();

        void on_create_clicked();
        void on_delete_clicked();
        void on_create_state_clicked();
        void on_create_resourcelock_clicked();
        void on_create_transition_clicked();

        void on_name_changed();
        void on_launch_on_start_changed();

        void new_entity_coords(float& x, float& y, bool offset);

        void _update_enabled();
        void _rebuild_list(unsigned int deleted_id);
};
