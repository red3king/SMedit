#pragma once

#include <vector>

#include "draw_context.h"
#include "gui/models/gui_model.h"
#include "signals.h"


class GUIState
{
    // Describes the state of the opengl box containing states, transitions, and resourcelocks
    //

    public:

        Machine* current_machine;
        DrawContext draw_context;
        vector<GUIModel*> gui_models;
        
        GUIModel* get_model_by_id(unsigned int id);

        void set_nvg_context(NVGcontext* vg);
        void set_machine(Machine* current_machine);
        void unset_machine();

        void pre_rebuild_models();
        void rebuild_models();

        void add_gui_model(EntityType entity_type, unsigned int entity_id);
        void remove_gui_model(unsigned int entity_id);

        CursorType update_models(CurrentEvents& current_events);
        
        void draw();

    private:
        void _delete_models();
        void _create_models();
};

