#pragma once

#include "gui_box_model.h"


class GMState : public GUIBoxModel
{
    public:
        GMState(DrawContext* ctx, unsigned int machine_id, unsigned int model_id);
}; 
