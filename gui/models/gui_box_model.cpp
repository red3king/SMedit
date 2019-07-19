#include "lib/nanovg/nanovg.h"

#include "gui_box_model.h"
#include "utils.h"
#include "log.h"

GMBox::GMBox(DrawContext* ctx, EntityType type) : GUIModel(ctx, type) { }


void GMBox::draw()
{
    float x, y, w, h, screen_x, screen_y, screen_w, screen_h;
    get_coords(x, y, w, h);

    float title_height = 21.0;
    float border_w = 4.0;

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
    nvgFillColor(vg, nvgRGBA(0x2f, 0x30, 0x32, 255));
    nvgFill(vg); 

    // interior 

    // title
    ctx->draw_text_noclip(get_title(), ctx->font_hack_bold, 16, nvgRGBA(0xe6, 0xe5, 0xca, 255), 
            screen_x + 4, screen_y + 4);
    draw_interior();
}


bool GMBox::mouse_in_range(float mouse_x, float mouse_y)
{
    return mouse_within(mouse_x, mouse_y);
}


bool GMBox::mouse_within(float mouse_x, float mouse_y)
{
    float screen_x, screen_y, screen_w, screen_h, x, y, w, h;
    get_coords(x, y, w, h);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(w);
    ctx->world_to_screen(screen_x, screen_y, x, y);

    return point_in_box(mouse_x, mouse_y, screen_x + BM_INT_GAP, screen_y + BM_INT_GAP, 
            screen_w - 2*BM_INT_GAP, screen_h - 2*BM_INT_GAP);
}


BorderType GMBox::mouse_on_border(float mouse_x, float mouse_y)
{
    float screen_x, screen_y, screen_w, screen_h, x, y, w, h;
    get_coords(x, y, w, h);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(w);
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

