#include "gui/colors.h"
#include "gui_transition.h"
#include "utils.h"


#define D0T .05
#define D1T .95
#define LST .1
#define LET .9
#define APT .7

#define EP_THRESH 10
#define LD_THRESH 7


GMTransition::GMTransition(DrawContext* ctx, Transition* transition) : GUIModel(ctx, TRANSITION)
{
    this->transition = transition;
}


void GMTransition::draw()
{
    /*
     
       |                      |
       | * ------------->-- * |
       |                      |
       x0                     x1
    */

    // Establish relavant coordinates
    float lsx, lsy, lex, ley;           // line start, end
    float apx, apy, alx, aly, arx, ary; // arrow point, arrow left, arrow right
    float d0x, d0y, d1x, d1y;           // dot 0, dot 1

    interp(D0T, d0x, d0y);
    interp(D1T, d1x, d1y);
    interp(LST, lsx, lsy);
    interp(LET, lex, ley);
    interp(APT, apx, apy);

    float theta = atan2(transition->y1 - transition->y0, transition->x1 - transition->x0);
    float theta_al = theta + .75 * M_PI;
    float theta_ar = theta + 1.25 * M_PI;
    move_in_direction(apx, apy, 12.0, theta_al, alx, aly);
    move_in_direction(apx, apy, 12.0, theta_ar, arx, ary);

    ctx->world_to_screen(lsx, lsy, lsx, lsy);
    ctx->world_to_screen(lex, ley, lex, ley);
    ctx->world_to_screen(apx, apy, apx, apy);
    ctx->world_to_screen(alx, aly, alx, aly);
    ctx->world_to_screen(arx, ary, arx, ary);
    ctx->world_to_screen(d0x, d0y, d0x, d0y);
    ctx->world_to_screen(d1x, d1y, d1x, d1y);


    // Draw arrow
    NVGcontext* vg = ctx->vg;
    nvgReset(vg);
    nvgBeginPath(vg);
    
    // draw line
    nvgMoveTo(vg, lsx, lsy);
    nvgLineTo(vg, lex, ley);
    
    // arrow
    nvgMoveTo(vg, apx, apy);
    nvgLineTo(vg, alx, aly);
    nvgMoveTo(vg, apx, apy);
    nvgLineTo(vg, arx, ary);

    // stroke it

    NVGcolor main_color = selected ? ORANGERED : GRAY;
    NVGcolor highlight_color = GRAYWHITE;

    if(mouse_over)
    {
        nvgStrokeColor(vg, highlight_color);
        nvgStrokeWidth(vg, 4);
        nvgStroke(vg);
    }

    nvgStrokeColor(vg, main_color);
    nvgStrokeWidth(vg, 2.5);
    nvgStroke(vg);

    // Draw dots
    nvgReset(vg);
    nvgBeginPath(vg);

    if(mouse_over)
    {
        nvgCircle(vg, d0x, d0y, 6);
        nvgCircle(vg, d1x, d1y, 6);
        nvgFillColor(vg, highlight_color);
        nvgFill(vg);
        nvgReset(vg);
        nvgBeginPath(vg);
    }

    nvgCircle(vg, d0x, d0y, 4);
    nvgCircle(vg, d1x, d1y, 4);
    nvgFillColor(vg, main_color);
    nvgFill(vg);
}


void GMTransition::interp(float t, float& x_out, float& y_out)
{
    x_out = transition->x0 + t * (transition->x1 - transition->x0);
    y_out = transition->y0 + t * (transition->y1 - transition->y0);
}


CursorType GMTransition::update_impl(CurrentEvents& current_events)
{
    return CT_DEFAULT;
}


Entity* GMTransition::get_entity()
{
    return transition;
}


bool GMTransition::mouse_within(float mouse_x, float mouse_y)
{
    return mouse_on_border(mouse_x, mouse_y) != TB_NONE;
}


TransitionBorder GMTransition::mouse_on_border(float mouse_x, float mouse_y)
{
    float mouse_wx, mouse_wy;
    ctx->screen_to_world(mouse_wx, mouse_wy, mouse_x, mouse_y);

    float d0x, d0y, d1x, d1y;
    interp(D0T, d0x, d0y);
    interp(D1T, d1x, d1y);
    
    if(distance(mouse_wx, mouse_wy, d0x, d0y) < EP_THRESH)
        return TB_BEGIN;

    if(distance(mouse_wx, mouse_wy, d1x, d1y) < EP_THRESH)
        return TB_END;

    float t, d;
    distance_to_line(transition->x0, transition->y0, transition->x1, transition->y1,
            mouse_wx, mouse_wy, t, d);

    return t > 0 && t < 1 && abs(d) < LD_THRESH ? TB_MID : TB_NONE;
}
