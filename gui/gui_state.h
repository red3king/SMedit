#pragma once

#include <vector>

#include "draw_context.h"
#include "gui/models/gui_model.h"


class GUIState
{
    // Describes the state of the opengl box containing states, transitions, and resourcelocks
    //

    public:

        unsigned int current_machine_id;    // id of the state machine thats being edited in the window
        DrawContext draw_context;
        vector<GUIModel*> gui_models;
        

        GUIModel* get_model_by_id(unsigned int id);
};
