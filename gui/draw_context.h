#pragma once

#include "lib/nanovg/nanovg.h"


class DrawContext
{
    public:
        DrawContext();
        void set_nvg_context(NVGcontext* vg);
        
        // transforms between screen coordinates in pixels and the world
        void world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y); 
        float world_dist_to_screen(float world_dist);
        void screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y);
        float screen_dist_to_world(float screen_dist);

        void move(float world_dx, float world_dy);  // move viewport, in world coords
        void zoom(float zoom_amount, float fp_screen_x, float fp_screen_y);  // zoom in/out from fixed point

        void reset();

        NVGcontext* vg;

        float zoom_factor, woffset_x, woffset_y;
};
