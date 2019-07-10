#pragma once

#include "gui_model.h"


class GUIBoxModel : public GUIModel
{
    public:
        GUIBoxModel(DrawContext* ctx, GUIModelType type, unsigned int machine_id, unsigned int model_id);
        virtual void get_coords(Project p, float& x, float& y, float& w, float& h);
        virtual void set_coords(Project p, float x, float y, float w, float h);
};

