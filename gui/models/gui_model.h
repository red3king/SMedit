#pragma once

#include "models/project.h"
#include "gui/draw_context.h"


enum GUIModelType { STATE, TRANSITION, RESOURCELOCK };


class GUIModel
{
    public:
        GUIModel(DrawContext* ctx, GUIModelType type, unsigned int machine_id,  unsigned int model_id);
        virtual ~GUIModel()=0;

        virtual void draw(const Project& p);
        virtual bool mouse_in_range(const Project& project, int x, int y);
        
        unsigned int model_id, machine_id;
        GUIModelType type;

    private:
        DrawContext* ctx;
};

