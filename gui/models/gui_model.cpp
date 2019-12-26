#include "gui_model.h"
#include "gui/colors.h"

#include "lib/nanovg/nanovg.h"


/// GMNotification stuff

GMNotification::GMNotification(DrawContext *ctx, int icon, string message)
{
    this->icon = icon;
    this->message = message;
    this->ctx = ctx;

    selected = false;
    id = ++_id_ctr;
    world_x = 0;
    world_y = 0;
}


int GMNotification::_id_ctr = 0;


void GMNotification::set_coordinates(float x, float y)
{
    world_x = x;
    world_y = y;
}


void GMNotification::check_mouse_within(float mouse_x, float mouse_y)
{
    ctx->screen_to_world(mouse_x, mouse_y, mouse_x, mouse_y);
    selected = mouse_x > world_x && mouse_x < world_x + ctx->screen_dist_to_world(NOTIF_WIDTH) &&
        mouse_y > world_y && mouse_y < world_y + ctx->screen_dist_to_world(NOTIF_HEIGHT);
}


void GMNotification::draw()
{
    auto vg = ctx->vg;
    float screen_x, screen_y;
    ctx->world_to_screen(screen_x, screen_y, world_x, world_y);
    NVGpaint paint = nvgImagePattern(vg, screen_x, screen_y, NOTIF_WIDTH, NOTIF_HEIGHT, 0, icon, 255.0);
    nvgBeginPath(vg);
    nvgRect(vg, screen_x, screen_y, NOTIF_WIDTH, NOTIF_HEIGHT);
    nvgFillPaint(vg, paint);
    nvgFill(vg);    
    
    if(selected)
        draw_message();
}


void GMNotification::draw_message()
{
    float screen_x, screen_y;
    ctx->world_to_screen(screen_x, screen_y, world_x, world_y);

    screen_x += NOTIF_WIDTH + NOTIF_GAP;
    screen_y += NOTIF_HEIGHT / 2.0;

    float tw, th;
    ctx->measure_text(tw, th, message, ctx->font_hack, NOTIF_FONT_SIZE, NOTIF_TEXT_WIDTH);
    tw += 2 * NOTIF_TEXT_OFFSET;
    th += 2 * NOTIF_TEXT_OFFSET;

    // calculate triangle coordinates
    float t_l_x = screen_x;
    float t_l_y = screen_y;
    
    float t_t_x = screen_x + NOTIF_GAP;
    float t_t_y = screen_y - NOTIF_GAP;
    
    float t_b_x = screen_x + NOTIF_GAP;
    float t_b_y = screen_y + NOTIF_GAP;
    
    // calculate box coordinates
    float b_tl_x = screen_x + NOTIF_GAP;
    float b_tl_y = screen_y - (th/2.0); 
    
    float b_tr_x = b_tl_x + tw;
    float b_tr_y = b_tl_y;
    
    float b_br_x = b_tr_x;
    float b_br_y = screen_y + (th/2.0);

    float b_bl_x = b_tl_x;
    float b_bl_y = b_br_y;

    // define border path
    float coords[] = {
        t_l_x, t_l_y,
        t_t_x, t_t_y,
        b_tl_x, b_tl_y,
        b_tr_x, b_tr_y,
        b_br_x, b_br_y,
        b_bl_x, b_bl_y,
        t_b_x, t_b_y,
        t_l_x, t_l_y
    };    

    // draw 

    auto vg = ctx->vg;
    nvgBeginPath(vg);
    nvgMoveTo(vg, coords[0], coords[1]);

    for(int i=1; i<8; i++)    
        nvgLineTo(vg, coords[2*i], coords[2*i+1]); 

    nvgClosePath(vg);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillColor(vg, ORANGERED);
    nvgFill(vg);

    // Text
    ctx->draw_text_many_lines(message, ctx->font_hack, NOTIF_FONT_SIZE, WHITE, 
            b_tl_x + NOTIF_TEXT_OFFSET, b_tl_y + NOTIF_TEXT_OFFSET, NOTIF_TEXT_WIDTH, 999);
}


/// GUIModel stuff


GUIModel::GUIModel(DrawContext* ctx, EntityType type)
{
    this->type = type;
    this->ctx = ctx;

    selected = false;
    mouse_over = false;

    // notifications.push_back(GMNotification(ctx, ctx->notif_icon_lock, "locked!"));
    // notifications.push_back(GMNotification(ctx, ctx->notif_icon_error, "help! i got stuck in a blender and I am dead! oh no! help! aaauuuughhhhhhhhhhsadasdasdljasdofiajsdfoiajsdofaijsd"));
}


unsigned int GUIModel::get_id()
{
    return get_entity()->id;
}


GUIModel::~GUIModel(){ }


void GUIModel::set_selected(bool selected)
{
    this->selected = selected;
}


void GUIModel::draw()
{
    draw_impl();

    if(!too_small())
        draw_notifications();
}


void GUIModel::draw_notifications()
{
    NVGcontext *vg = ctx->vg;

    for(int i=0; i<notifications.size(); i++)
        notifications[i].draw();
}


void GUIModel::update_notifications(CurrentEvents& current_events)
{
    float wx, wy;
    get_notification_coordinates(wx, wy);

    for(int i=0; i<notifications.size(); i++)
    {
        notifications[i].set_coordinates(wx, wy);
        notifications[i].check_mouse_within(current_events.mouse_x, current_events.mouse_y);
        wy += ctx->screen_dist_to_world(NOTIF_GAP + NOTIF_HEIGHT);        
    } 
}


CursorType GUIModel::update(CurrentEvents& current_events, GUIModel*& just_selected)
{
    update_notifications(current_events);
    mouse_over = mouse_within(current_events.mouse_x, current_events.mouse_y);

    if(mouse_over)
    {
        bool clicked = current_events.event_type == ET_MB_PRESS && current_events.button_pressed(MB_LEFT);

        // If something else has already been selected because they're overlapping
        // don't become selected.
        if(clicked && just_selected == nullptr)
        {
            selected = true;
            just_selected = this;
        }
    }

    return update_impl(current_events);
}
