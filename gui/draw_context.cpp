#include "draw_context.h"


#define ZFAC 1.2


DrawContext::DrawContext(NVGcontext* vg)
{
    this->vg = vg;
    woffset_x = 0;
    woffset_y = 0;
    zoom_factor = 1;
}


void DrawContext::set_size(int widget_width, int widget_height)
{
    this->widget_width = widget_width;
    this->widget_height = widget_height;
}


void DrawContext::world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y)
{
    screen_x = zoom_factor * (world_x - woffset_x);
    screen_y = zoom_factor * (world_y - woffset_y);
}


void DrawContext::screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y)
{
    world_x = (screen_x / zoom_factor) + woffset_x;
    world_y = (screen_y / zoom_factor) + woffset_y;
}


void zoom_in()
{
    zoom_factor *= ZFAC;
}


void zoom_in()
{
    zoom_factor /= ZFAC;
}

