#include "gui_resourcelock.h"


GMResourceLock::GMResourceLock(DrawContext* ctx, ResourceLock* resourcelock) : GMBox(ctx, RESOURCELOCK)
{
    this->resourcelock = resourcelock;
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


bool GMResourceLock::get_icon(int& icon_image)
{
    icon_image = ctx->icon_lock;
    return true;
}
