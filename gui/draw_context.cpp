#include "draw_context.h"


#define ZFAC 1.2


DrawContext::DrawContext()
{
    reset();
}


void DrawContext::set_nvg_context(NVGcontext* vg)
{
    this->vg = vg;
}


void DrawContext::reset()
{
    woffset_x = 0;
    woffset_y = 0;
    zoom_factor = 1;
}


void DrawContext::world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y)
{
    screen_x = zoom_factor * (world_x - woffset_x);
    screen_y = zoom_factor * (world_y - woffset_y);
}


float DrawContext::world_dist_to_screen(float world_dist)
{
    return world_dist * zoom_factor;
}


void DrawContext::screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y)
{
    world_x = (screen_x / zoom_factor) + woffset_x;
    world_y = (screen_y / zoom_factor) + woffset_y;
}


float DrawContext::screen_dist_to_world(float screen_dist)
{
    return screen_dist / zoom_factor;
}


void DrawContext::zoom(float zoom_amount, float fp_screen_x, float fp_screen_y)
{
    float wx1, wx2, wy1, wy2, dx, dy;

    screen_to_world(wx1, wy1, fp_screen_x, fp_screen_y);
    zoom_factor *= (1 + zoom_amount/10.0);
    screen_to_world(wx2, wy2, fp_screen_x, fp_screen_y);

    dx = wx2 - wx1;
    dy = wy2 - wy1;
    woffset_x -= dx;
    woffset_y -= dy;
}


