#pragma once

#include <gtkmm.h>

#include "signals.h"
#include "current_events.h"
#include "gui_state.h"
#include "historymanager/historymanager.h"
#include "operations/gui_operation.h"
#include "banner_display.h"
#include "controllers/run/run_state.h"


class GUIContext
{
    // Contains all the state and stuff relating to a GLArea,
    // except the models (transitions/states/resourcelocks contained inside the GUIState object)
    // TODO name these better?

    public:
        GUIContext(Gtk::GLArea* gl_area, HistoryManager* history_manager, GUIAreaMode execution_mode, RunningState* running_state=nullptr, BannerDisplayer* banner_displayer=nullptr); // RunningState & BannerDisplayer only for GAM_RUN

        GUIAreaMode mode; // are we a place for the user to build a state machine,
                          // or a place to watch it run?

        unsigned int current_machine_id;

        void set_machine(Project* current_project, Machine* current_machine, int running_machine_id = -1);
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
        void handle_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type);

        // RunningState signals
        void rs_hndl_select_machine(int machine_id, Project* project, Machine* machine_def);
        void rs_hndl_select_state(int state_def_id);
          
        void update();

        Gtk::GLArea* gl_area;
        NVGcontext* vg;

        BannerDisplayer* banner_displayer;

        CurrentEvents current_events;
        GUIState gui_state;
        GUIOperation* current_operation;
        Machine* current_machine;
        Project* current_project;

        CursorType current_cursor_type;

        HistoryManager* history_manager;

    private:
        
        RunningState* running_state;

        void calc_to_state_zoom(State* state, float& target_x, float& target_y, float& target_zoom);

        void on_focus_operation(Operation* operation, unsigned int result);
        
        bool has_current_operation();
        void register_gtk_signal_handlers();
        void register_sm_signal_handlers();
        void unregister_sm_signal_handlers();
        void _render();
};
