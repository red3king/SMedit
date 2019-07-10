#pragma once

#include "lib/nanovg/nanovg.h"


class DrawContext
{
    public:
        DrawContext(NVGcontext* vg);

        // transforms between screen coordinates in pixels and the world
        void world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y); 
        void screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y);

        void zoom_in();
        void zoom_out();

        NVGcontext* vg;
        int widget_width, widget_height;

        float zoom_factor, woffset_x, woffset_y;
};
