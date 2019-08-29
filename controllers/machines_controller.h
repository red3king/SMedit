#pragma once

#include "list_view_controller.h"
#include "top_controller.h"
#include "selected_item/selected_resourcelock.h"
#include "selected_item/selected_transition.h"
#include "selected_item/selected_state.h"
#include "historymanager/historymanager.h"
#include "models/machine.h"
#include "gui/draw_context.h"
#include "signals.h"


class MachinesController : public TopController
{
    public:
        MachinesController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

    private:
        ListViewController* list_view_controller;
        SelectedResourceLock* selected_resourcelock_ctrl;
        SelectedTransition* selected_transition_ctrl;
        SelectedState* selected_state_ctrl;

        DrawContext* draw_context;
        GUIContext* gui_context;

        // last selected entity
        unsigned int last_entity_id, last_machine_id;
        EntityType last_entity_type;
 
        // ui state
        Machine* selected_machine;
        int selected_machine_id;
        bool disable_input_signals;
        bool create_clicked;

        // widgets
        Gtk::TreeView* tree_view;
        Gtk::Button *create_button, *delete_button;
        Gtk::Button *create_transition_button, *create_state_button, *create_resourcelock_button;
        Gtk::Label *name_label, *launch_on_start_label;
        Gtk::Entry* name_entry;
        Gtk::Switch* launch_on_start_switch;
        Gtk::Stack* selected_item_stack;
        Gtk::GLArea* gl_area;

        // signal handlers
        void on_model_selected(Machine* machine, EntityType entity_type, Entity* entity);
        void on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);
        void on_selection_changed(unsigned int machine_id, bool user_generated);
        void _populate_ui_from_selected();

        void reset(bool reload);
        void load_from(Project& current_project, bool reload);

        void _do_select(Machine* machine, EntityType entity_type, Entity* entity);

        // ui element ev handlers
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
