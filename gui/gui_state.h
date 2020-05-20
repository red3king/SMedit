#pragma once

#include <vector>

#include "draw_context.h"
#include "models/gui_model.h"
#include "banner_display.h"
#include "signals.h"
#include "controllers/run/run_state.h"


enum GUIAreaMode { GAM_BUILD, GAM_RUN };


class GUIState
{
    // Describes the state of the opengl box containing states, transitions, and resourcelocks

    public:
        GUIState(GUIAreaMode execution_mode, RunningState* running_state, 
                BannerDisplayer* banner_displayer, Gtk::GLArea* gl_area);

        GUIAreaMode mode;
        Gtk::GLArea* gl_area;

        DrawContext draw_context;
        RunningState* running_state;
        Machine* current_machine;
        Project* current_project;
        vector<GUIModel*> gui_models;
        
        void set_nvg_context(NVGcontext* vg);
        void restore_selected_entity(Entity* entity);

        void set_machine(Project* current_project, Machine* current_machine);
        void unset_machine();

        void add_gui_model(EntityType entity_type, unsigned int entity_id);
        void remove_gui_model(unsigned int entity_id);
        
        CursorType update_models(CurrentEvents& current_events, GUIModel*& just_selected, bool& clear_selected);
        void draw();

        // GAM_RUN animation stuff
        void rs_state_select(int state_def_id);

    private:
        BannerDisplayer* banner_displayer;  // GAM_RUN text displayer thingy

        void delete_models();
        void create_models();
        
        void update_lock_notifications();
        
        void on_focus_operation(Operation* operation, unsigned int result);
        void on_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type);

        GUIModel* get_model_by_id(unsigned int id);
        
        bool has_machine();
};

