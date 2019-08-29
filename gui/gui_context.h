#pragma once

#include <gtkmm.h>

#include "signals.h"
#include "current_events.h"
#include "gui_state.h"
#include "historymanager/historymanager.h"
#include "gui/operations/gui_operation.h"


class GUIContext
{
    // Contains all the state and stuff relating to a GLArea

    public:
        GUIContext(Gtk::GLArea* gl_area, HistoryManager* history_manager);

        unsigned int current_machine_id;
        void set_machine(Machine* current_machine);
        void unset_machine();
        
        void reset(bool reload);
        void load_from(Project& current_project, bool reload);

        void restore_selected_entity(Entity* entity);

        // Gdk event handlers
        void on_gl_realize();
        void on_gl_unrealize();
        bool on_gl_render(const Glib::RefPtr<Gdk::GLContext>& context);

        bool handle_button_event(GdkEventButton* button_event);
        bool handle_scroll_event(GdkEventScroll* scroll_event);
        bool handle_key_event(GdkEventKey* key_event);
        bool handle_motion_notify_event(GdkEventMotion* motion_event);

        // smedit signal handlers
        void handle_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);

        void update();

        Gtk::GLArea* gl_area;
        NVGcontext* vg;

        CurrentEvents current_events;
        GUIState gui_state;
        GUIOperation* current_operation;
        Machine* current_machine;

        CursorType current_cursor_type;

        HistoryManager* history_manager;

    private:
        bool has_current_operation();
        void register_gtk_signal_handlers();
        void register_sm_signal_handlers();
        void unregister_sm_signal_handlers();
        void _render();
};
