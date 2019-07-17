#include "lib/nanovg/nanovg.h"

#include "gui_box_model.h"
#include "utils.h"
#include "log.h"

GMBox::GMBox(DrawContext* ctx, EntityType type) : GUIModel(ctx, type) { }


void GMBox::draw()
{
    float x, y, w, h, screen_x, screen_y, screen_w, screen_h;
    get_coords(x, y, w, h);

    ctx->world_to_screen(screen_x, screen_y, x, y);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);

    NVGcontext* vg = ctx->vg;

    nvgBeginPath(vg);
    nvgRoundedRect(vg, screen_x, screen_y, screen_w, screen_h, 2);
    nvgFillColor(vg, nvgRGBA(200, 200, 200, 255));
    nvgFill(vg); 
    
    draw_interior();
}


bool GMBox::mouse_in_range(float mouse_x, float mouse_y)
{
    return mouse_within(mouse_x, mouse_y);
}


bool GMBox::mouse_within(float mouse_x, float mouse_y)
{
    float mouse_x_world, mouse_y_world, x, y, w, h;
    ctx->screen_to_world(mouse_x_world, mouse_y_world, mouse_x, mouse_y);
    get_coords(x, y, w, h);

    return point_in_box(mouse_x_world, mouse_y_world, x + BM_INT_GAP, y + BM_INT_GAP, w - BM_INT_GAP, h - BM_INT_GAP);
}


bool GMBox::mouse_on_border(float mouse_x, float mouse_y)
{
    float mouse_x_world, mouse_y_world, x, y, w, h;
    get_coords(x, y, w, h);
    ctx->screen_to_world(mouse_x_world, mouse_y_world, mouse_x, mouse_y);
    
    return point_in_box(mouse_x_world, mouse_y_world, x - BM_INT_GAP, y - BM_INT_GAP,
            w + BM_INT_GAP, h + BM_INT_GAP) && !mouse_within(mouse_x, mouse_y);
};

