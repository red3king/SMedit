#pragma once

#include <vector>

#include "draw_context.h"
#include "gui/models/gui_model.h"
#include "signals.h"


class GUIState
{
    // Describes the state of the opengl box containing states, transitions, and resourcelocks

    public:
        DrawContext draw_context;
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

    private:
        void delete_models();
        void create_models();

        GUIModel* get_model_by_id(unsigned int id);
};

