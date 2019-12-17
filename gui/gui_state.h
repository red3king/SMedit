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

        DrawContext draw_context;
        RunningState* running_state;
        Machine* current_machine;
        vector<GUIModel*> gui_models;
        
        void set_nvg_context(NVGcontext* vg);
        void restore_selected_entity(Entity* entity);

        void set_machine(Machine* current_machine);
        void unset_machine();

        void add_gui_model(EntityType entity_type, unsigned int entity_id);
        void remove_gui_model(unsigned int entity_id);
        
        CursorType update_models(CurrentEvents& current_events, GUIModel*& just_selected, bool& clear_selected);
        void draw();

        // GAM_RUN animation stuff
        void rs_state_select(int state_def_id);

    private:
        BannerDisplayer* banner_displayer;

        void delete_models();
        void create_models();

        GUIModel* get_model_by_id(unsigned int id);
};

