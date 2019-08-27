#include "lib/nanovg/nanovg.h"

#include "gui/colors.h"
#include "gui_box_model.h"
#include "utils.h"
#include "log.h"


#define ICON_SIZE 30
#define ICON_MARGIN 4
#define BORDER_W 4
#define TEXT_LINE_HEIGHT 18


GMBox::GMBox(DrawContext* ctx, EntityType type) : GUIModel(ctx, type) 
{ 
    text_line_no = 0;
    text_line_pos = 0;
}


void GMBox::get_coords(float& x, float& y, float& w, float& h)
{
    auto entity = (BoxEntity*) get_entity();
    x = entity->x;
    y = entity->y;
    w = entity->w;
    h = entity->h;
}


void GMBox::set_coords(float x, float y, float w, float h)
{
    auto entity = (BoxEntity*) get_entity();
    entity->x = x;
    entity->y = y;
    entity->w = w;
    entity->h = h;
}


void GMBox::draw()
{
    text_line_no = 0;

    float x, y, w, h, screen_x, screen_y, screen_w, screen_h;
    get_coords(x, y, w, h);
    float title_height = ICON_SIZE + 2*ICON_MARGIN;
    float old_title_height = 21;

    float corner_radius = 5.0;

    ctx->world_to_screen(screen_x, screen_y, x, y);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);

    NVGcontext* vg = ctx->vg;
    nvgReset(vg);

    // interior 
    draw_interior(screen_x + BORDER_W, screen_y + old_title_height, screen_w - 2*BORDER_W, 
            screen_h - BORDER_W - old_title_height);


    // Draw border and title bar
    nvgBeginPath(vg);
    nvgRoundedRect(vg, screen_x, screen_y, screen_w, screen_h, 5.0);

    float xs[] = {
        screen_x + BORDER_W,
        screen_x + ICON_SIZE + 2*ICON_MARGIN,
        screen_x + ICON_SIZE + 2*ICON_MARGIN + 3,
        screen_x + screen_w - BORDER_W,
        screen_x + screen_w - BORDER_W,
        screen_x + BORDER_W,
        screen_x + BORDER_W
    };

    float ys[] = {
        screen_y + title_height,
        screen_y + title_height,
        screen_y + old_title_height,
        screen_y + old_title_height,
        screen_y + screen_h - BORDER_W,
        screen_y + screen_h - BORDER_W,
        screen_y + title_height
    };

    nvgMoveTo(vg, xs[0], ys[0]);

    for(int i=1; i<7; i++)
        nvgLineTo(vg, xs[i], ys[i]);

    nvgClosePath(vg);    
    nvgPathWinding(vg, NVG_HOLE);    
    
    nvgFillColor(vg, selected ? ORANGERED : GRAY);
    nvgFill(vg); 

    // Icon 
    int icon_image;
    if(get_icon(icon_image))
    {
        int ix = screen_x + ICON_MARGIN;
        int iy = screen_y + ICON_MARGIN;
        NVGpaint paint = nvgImagePattern(vg, ix, iy, ICON_SIZE, ICON_SIZE, 0, icon_image, 255.0);
        nvgBeginPath(vg);
        nvgRect(vg, ix, iy, 30, 30);
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    // mouse hover shadow thing
    if(mouse_over)
    {
        NVGpaint pt;
        float xtra = 3.3;
        int n = 2.25;
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
    ctx->draw_text_one_line(get_title(), ctx->font_hack_bold, 16, WHITE, 
            screen_x + 2*ICON_MARGIN + BORDER_W + ICON_SIZE, screen_y + BORDER_W, screen_w - ICON_SIZE - ICON_MARGIN);
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



bool GMBox::text(string text, NVGcolor color, bool stay)
{
    // type text into interior of box after the last text typed
    // if stay = false, go to new line

    float screen_x, screen_y, screen_w, screen_h, x, y, w, h;
    get_coords(x, y, w, h);
    screen_w = ctx->world_dist_to_screen(w);
    screen_h = ctx->world_dist_to_screen(h);
    ctx->world_to_screen(screen_x, screen_y, x, y);

    float tx = screen_x + BORDER_W + text_line_pos + 2;
    float ty = screen_y + 2*ICON_MARGIN + ICON_SIZE + TEXT_LINE_HEIGHT*text_line_no;
    float tw = screen_w - 2*BORDER_W - text_line_pos;

    if(ty + TEXT_LINE_HEIGHT > screen_y + screen_h)
        return false;

    ctx->draw_text_one_line(text, ctx->font_hack_bold, 16, color, tx, ty, tw); 

    if(!stay)
    {
        text_line_no++;
        text_line_pos = 0;
    }

    else
        text_line_pos += ctx->measure_text(text, ctx->font_hack_bold, 16);

    return true;
}
