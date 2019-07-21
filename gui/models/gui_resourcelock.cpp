#include "gui_resourcelock.h"


GMResourceLock::GMResourceLock(DrawContext* ctx, ResourceLock* resourcelock) : GMBox(ctx, RESOURCELOCK)
{
    this->resourcelock = resourcelock;
}


void GMResourceLock::get_coords(float& x, float& y, float& w, float& h)
{
    x = resourcelock->x;
    y = resourcelock->y;
    w = resourcelock->w;
    h = resourcelock->h;
}


void GMResourceLock::set_coords(float x, float y, float w, float h)
{
    resourcelock->x = x;
    resourcelock->y = y;
    resourcelock->w = w;
    resourcelock->h = h;
}


Entity* GMResourceLock::get_entity()
{
    return resourcelock;
}


void GMResourceLock::draw_interior(float x, float y, float w, float h)
{

}


NVGcolor GMResourceLock::get_border_color()
{
    return nvgRGBA(50, 240, 50, 255);
}


NVGcolor GMResourceLock::get_title_text_color()
{
    return nvgRGBA(255, 255, 255, 255);
}


std::string GMResourceLock::get_title()
{
    return "Lock [" + resourcelock->resource->name + "]";
}
