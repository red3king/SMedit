#include "lib/nanovg/nanovg.h"

#include "gui/colors.h"
#include "gui_box_model.h"
#include "utils.h"
#include "log.h"


GMBox::GMBox(DrawContext* ctx, EntityType type) : GUIModel(ctx, type) 
{ 
    mouse_over = false;
}


void GMBox::draw()
{
    float x, y, w, h, screen_x, screen_y, screen_w, screen_h;
    get_coords(x, y, w, h);
    float title_height = 21.0;
    float border_w = 4.0;

    float corner_radius = 5.0;

    ctx->world_to_screen(screen_x, screen_y, x, y);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);

    NVGcontext* vg = ctx->vg;
    nvgReset(vg);

    // Draw border and title bar
    nvgBeginPath(vg);
    nvgRoundedRect(vg, screen_x, screen_y, screen_w, screen_h, 5.0);
    nvgRect(vg, screen_x + border_w, screen_y + title_height, screen_w - 2*border_w, 
            screen_h - border_w - title_height);
    nvgPathWinding(vg, NVG_HOLE);    
    nvgFillColor(vg, selected ? ORANGERED : GRAY);
    nvgFill(vg); 

    // mouse hover shadow thing
    if(mouse_over)
    {
        NVGpaint pt;
        float xtra = 2.5;
        int n = 1.95;
        NVGcolor in_color = nvgRGBA(159, 159, 159, 222);
        NVGcolor out_color = nvgRGBA(0, 0, 0, 0);
        pt = nvgBoxGradient(vg, screen_x-xtra, screen_y-xtra, screen_w+2*xtra, screen_h+2*xtra, 
                n*corner_radius, 10, in_color, out_color);

        nvgBeginPath(vg);
        nvgRoundedRect(vg, screen_x-xtra, screen_y-xtra, screen_w+2*xtra, screen_h+2*xtra, n*corner_radius);
        nvgRoundedRect(vg, screen_x, screen_y, screen_w, screen_h, corner_radius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, pt);
        nvgFill(vg);
    }

    // title
    ctx->draw_text_noclip(get_title(), ctx->font_hack_bold, 16, WHITE, 
            screen_x + 4, screen_y + 4);

    // interior 
    draw_interior(screen_x + border_w, screen_y + title_height, screen_w - 2*border_w, 
            screen_h - border_w - title_height);
}


bool GMBox::mouse_within(float mouse_x, float mouse_y)
{
    float screen_x, screen_y, screen_w, screen_h, x, y, w, h;
    get_coords(x, y, w, h);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);
    ctx->world_to_screen(screen_x, screen_y, x, y);

    return point_in_box(mouse_x, mouse_y, screen_x + BM_INT_GAP, screen_y + BM_INT_GAP, 
            screen_w - 2*BM_INT_GAP, screen_h - 2*BM_INT_GAP);
}


BorderType GMBox::mouse_on_border(float mouse_x, float mouse_y)
{
    float screen_x, screen_y, screen_w, screen_h, x, y, w, h;
    get_coords(x, y, w, h);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);
    ctx->world_to_screen(screen_x, screen_y, x, y);

    if(point_in_box(mouse_x, mouse_y, screen_x, screen_y - BM_INT_GAP, screen_w, 2 * BM_INT_GAP))
        return BT_TOP;  

    if(point_in_box(mouse_x, mouse_y, screen_x + screen_w - BM_INT_GAP, screen_y, 2 * BM_INT_GAP, screen_h))
        return BT_RIGHT;

    if(point_in_box(mouse_x, mouse_y, screen_x, screen_y + screen_h - BM_INT_GAP, screen_w, 2*BM_INT_GAP))
        return BT_BOTTOM;

    if(point_in_box(mouse_x, mouse_y, screen_x - BM_INT_GAP, screen_y, 2*BM_INT_GAP, screen_h))
        return BT_LEFT;

    return BT_NONE;    
};


CursorType GMBox::update_impl(CurrentEvents& current_events)
{
    switch(mouse_on_border(current_events.mouse_x, current_events.mouse_y))
    {
        case BT_TOP:
        case BT_BOTTOM:
            return CT_RS_NS;
        case BT_RIGHT:
        case BT_LEFT:
            return CT_RS_EW;
    }
    
    if(mouse_over)
        return CT_MOVE;

    return CT_DEFAULT;    
}
