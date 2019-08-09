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

    // TODO - plan is to:
    //      make this class have onee of everything: gui_state, draw_context, etc
    //      this class takes in a glarea, and connects its signals and stuff
    //      this class will have the signal handlers i was putting into glarea_signal_handlers
    //      
    //      delete the single instance of gui_state i made, and replace it with a single instance of this.
    //

    public:
        GUIContext(Gtk::GLArea* gl_area, HistoryManager* history_manager);

        void set_machine(Machine* current_machine);
        void unset_machine();

        // Gdk event handlers
        void on_gl_realize();
        void on_gl_unrealize();
        bool on_gl_render(const Glib::RefPtr<Gdk::GLContext>& context);

        bool handle_button_event(GdkEventButton* button_event);
        bool handle_scroll_event(GdkEventScroll* scroll_event);
        bool handle_key_event(GdkEventKey* key_event);
        bool handle_motion_notify_event(GdkEventMotion* motion_event);

        // smedit signal handlers
        void handle_create(EntityType entity_type, SignalType signal_type, unsigned int entity_id);
        void handle_delete(EntityType entity_type, SignalType signal_type, unsigned int entity_id);
        void handle_gui_rebuild();
        void handle_pre_gui_rebuild();

        void update();

        Gtk::GLArea* gl_area;
        NVGcontext* vg;

        CurrentEvents current_events;
        GUIState gui_state;
        GUIOperation* current_operation;

        CursorType current_cursor_type;

        HistoryManager* history_manager;

    private:
        bool has_current_operation();
        void register_gtk_signal_handlers();
        void register_sm_signal_handlers();
        void unregister_sm_signal_handlers();
        void _render();
};
