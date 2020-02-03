#include "gui/colors.h"
#include "gui_transition.h"
#include "utils.h"


// Positions of graphical components
#define APT .7      // arrow point
#define TR_EP_OFFSET 9  // px offset of circles from line endpoints
#define LE_OFFSET 33     // px offset of line segment


// Highlight distances
#define EP_THRESH 12
#define LD_THRESH 8

// Arc mode
#define D0 150.0
#define R1 50.0

// Dot size
#define DOT_SIZE 8


GMTransition::GMTransition(DrawContext* ctx, Transition* transition) : GUIModel(ctx, TRANSITION)
{
    this->transition = transition;
}


bool GMTransition::is_arc()
{
    return distance(transition->x0, transition->y0, transition->x1, transition->y1) < D0;
}


bool GMTransition::is_child_transition()
{
    return transition->is_child_transition;
}

void GMTransition::draw_impl()
{
    /*
       |                      |
       | * ------------->-- * |
       |                      |
       x0                     x1
    */

    if(is_arc())
        draw_as_arc();
    else
        draw_as_line();

    float d0t, d1t, d0x, d0y, d1x, d1y;           // dot 0, dot 1 
    get_ts(d0t, d1t, TR_EP_OFFSET);
    interp(d0t, d0x, d0y);
    interp(d1t, d1x, d1y);
    ctx->world_to_screen(d0x, d0y, d0x, d0y);
    ctx->world_to_screen(d1x, d1y, d1x, d1y);

    // Draw dots

    NVGcontext* vg = ctx->vg;
    nvgReset(vg);
    nvgBeginPath(vg);

    if(mouse_over)
    {
        nvgCircle(vg, d0x, d0y, DOT_SIZE);
        nvgCircle(vg, d1x, d1y, DOT_SIZE);
        nvgFillColor(vg, get_highlight_color());
        nvgFill(vg);
        nvgReset(vg);
        nvgBeginPath(vg);
    }

    NVGcolor from_color = transition->from_connected() ? WHITE : get_main_color();
    NVGcolor to_color = transition->to_connected() ? WHITE : get_main_color();

    nvgCircle(vg, d0x, d0y, 4);
    nvgFillColor(vg, from_color);
    nvgFill(vg);
    
    nvgReset(vg);
    nvgBeginPath(vg);
    nvgCircle(vg, d1x, d1y, 4);
    nvgFillColor(vg, to_color);
    nvgFill(vg);


    // Draw arrow
    float apx, apy, alx, aly, arx, ary; 
    float theta = interp(APT, apx, apy);
    float theta_al = theta + .75 * M_PI;
    float theta_ar = theta + 1.25 * M_PI;

    float arclen = 12.0;
    arclen = ctx->screen_dist_to_world(arclen);
    move_in_direction(apx, apy, arclen, theta_al, alx, aly);
    move_in_direction(apx, apy, arclen, theta_ar, arx, ary);
    ctx->world_to_screen(apx, apy, apx, apy);
    ctx->world_to_screen(alx, aly, alx, aly);
    ctx->world_to_screen(arx, ary, arx, ary);

    nvgReset(vg);
    nvgBeginPath(vg);
 
    // arrow
    nvgMoveTo(vg, apx, apy);
    nvgLineTo(vg, alx, aly);
    nvgMoveTo(vg, apx, apy);
    nvgLineTo(vg, arx, ary);

    main_color_stroke();

    // text
    float tx, ty;
    interp(.5, tx, ty);
    ctx->world_to_screen(tx, ty, tx, ty);
    ctx->draw_text_noclip(transition->describe(), ctx->font_hack_bold, 16, WHITE, tx, ty);
}


bool GMTransition::too_small()
{
    if(is_arc())
    {
        float cx, cy, r, a0, a1;
        int dir;
        calc_arc_params(cx, cy, r, a0, a1, dir);
        return ctx->world_dist_to_screen(2*r) < 50;
    }

    float len = transition->get_length();
    return ctx->world_dist_to_screen(len) < 50;
}


void GMTransition::get_notification_coordinates(float& world_x, float& world_y)
{
    // right below text
    interp(.5, world_x, world_y);
    world_y += ctx->screen_dist_to_world(21);
}


void GMTransition::draw_as_line()
{
    // Establish relavant coordinates
    float lsx, lsy, lex, ley;           // line start, end
    float lst, let;

    get_ts(lst, let, LE_OFFSET);
    interp(lst, lsx, lsy);
    interp(let, lex, ley);

    ctx->world_to_screen(lsx, lsy, lsx, lsy);
    ctx->world_to_screen(lex, ley, lex, ley);

    NVGcontext* vg = ctx->vg;
    nvgReset(vg);
    nvgBeginPath(vg);
    
    // draw line
    nvgMoveTo(vg, lsx, lsy);
    nvgLineTo(vg, lex, ley);
    
    main_color_stroke();
}


void GMTransition::main_color_stroke()
{
    // stroke it
    NVGcontext* vg = ctx->vg;
    
    if(mouse_over)
    {
        nvgStrokeColor(vg, get_highlight_color());
        nvgStrokeWidth(vg, 4);
        nvgStroke(vg);
    }

    nvgStrokeColor(vg, get_main_color());
    nvgStrokeWidth(vg, 2.5);
    nvgStroke(vg);
}


bool GMTransition::arc_loop_upwards()
{
    State* connected_state = nullptr;
    float connected_y, connected_x;

    if(transition->from_state != nullptr)
    {
        connected_state = transition->from_state;
        connected_y = transition->y0;
        connected_x = transition->x0;
    }

    else if(transition->to_state != nullptr)
    {
        connected_state = transition->to_state;
        connected_y = transition->y1;
        connected_x = transition->x1;
    }

    if(connected_state == nullptr)
        return true;
    
    return (connected_y < (connected_state->y + 0.5 * connected_state->h));   
};


void GMTransition::calc_arc_params(float& cx, float& cy, float& r, float& a0, float& a1, int& dir)
{
    bool flip = arc_loop_upwards();

    float x0 = transition->x0;
    float x1 = transition->x1;
    float y0 = transition->y0;
    float y1 = transition->y1;

    float d = distance(x0, y0, x1, y1);
    float h = R1 * D0 / (D0 - d) - 2 * R1;

    dir = NVG_CW;
    if(flip)
    {
        h *= -1;
        dir = NVG_CCW;
    }

    r = sqrt((pow(((R1 * D0 / (D0 - d)) - 2 * R1), 2)) + (d * d / 4.0));

    float theta = atan2(y1 - y0, x1 - x0);
    interp(.5, cx, cy, true);
    
    float move_angle = theta - M_PI / 2.0;
    move_in_direction(cx, cy, h, move_angle, cx, cy);
    
    if(d == 0 && flip)
    {
        a0 = M_PI / 2.0;
        a1 = a0;
        return;
    }

    float dtheta = acos(h / r);
    a0 = theta + M_PI / 2.0 - dtheta;
    a1 = theta + M_PI / 2.0 + dtheta;
}


void GMTransition::draw_as_arc()
{
    float cx, cy, r, a0, a1;
    int dir;

    calc_arc_params(cx, cy, r, a0, a1, dir);

    // convert to screen coords
    ctx->world_to_screen(cx, cy, cx, cy);
    r = ctx->world_dist_to_screen(r);

    // draw
    NVGcontext* vg = ctx->vg;
    nvgReset(vg);
    nvgBeginPath(vg);

    // Add 2pi because nanovg won't go in a circle from theta to theta
    if(abs(a0 - a1) < .0001)
        a0 += 2.0 * M_PI;

    nvgArc(vg, cx, cy, r, a0, a1, dir);
    main_color_stroke();
}


NVGcolor GMTransition::get_main_color()
{
    return selected ? ORANGERED : GRAY;
}


NVGcolor GMTransition::get_highlight_color()
{
    return GRAYWHITE;
}


float GMTransition::interp(float t, float& x_out, float& y_out, bool force_linear)  //force_linear default=false.
{
    if(force_linear || !is_arc())
    {
        x_out = transition->x0 + t * (transition->x1 - transition->x0);
        y_out = transition->y0 + t * (transition->y1 - transition->y0);
        return atan2(transition->y1 - transition->y0, transition->x1 - transition->x0);
    }

    float cx, cy, r, a0, a1;
    int dir;
    calc_arc_params(cx, cy, r, a0, a1, dir);

    float theta;
    if(arc_loop_upwards())
        theta = a1 + t * (2.0 * M_PI + a0 - a1);
    else
        theta = a1 + t * (a0 - a1);

    move_in_direction(cx, cy, r, theta, x_out, y_out);

    return theta + (dir == NVG_CW ? -M_PI/2.0 : M_PI/2.0);
}


CursorType GMTransition::update_impl(CurrentEvents& current_events)
{
    TransitionBorder bord = mouse_on_border(current_events.mouse_x, current_events.mouse_y);
    if(!transition->any_connected() && bord != TB_NONE)
        return CT_MOVE;

    if(bord == TB_BEGIN || bord == TB_END)
        return CT_MOVE;

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


void GMTransition::get_ts(float& d0t, float& d1t, float soffset)
{
    if(transition->is_loopback() || is_arc())
    {
        d0t = 0.0;
        d1t = 1.0;
        return;
    }
    
    float w_len = transition->get_length();
    float w_desired = ctx->screen_dist_to_world(soffset);
    float frac = w_desired / w_len;

    d0t = transition->from_connected() ? frac : 0.0;
    d1t = transition->to_connected() ? 1.0 - frac : 1.0;

    if(d0t > .2)
        d0t = .2;

    if(d1t < .8)
        d1t = .8;
}


TransitionBorder GMTransition::mouse_on_border(float mouse_x, float mouse_y)
{
    float mouse_wx, mouse_wy;
    ctx->screen_to_world(mouse_wx, mouse_wy, mouse_x, mouse_y);

    float d0t, d1t, d0x, d0y, d1x, d1y;
    get_ts(d0t, d1t, TR_EP_OFFSET);
    interp(d0t, d0x, d0y);
    interp(d1t, d1x, d1y);
    
    if(distance(mouse_wx, mouse_wy, d0x, d0y) < EP_THRESH)
        return TB_BEGIN;

    if(distance(mouse_wx, mouse_wy, d1x, d1y) < EP_THRESH)
        return TB_END;

    if(is_arc())
    {
        float cx, cy, r, a0, a1;
        int dir;
        calc_arc_params(cx, cy, r, a0, a1, dir);
        
        if(abs(r - distance(cx, cy, mouse_wx, mouse_wy)) > 5)
            return TB_NONE;

        float theta = atan2(mouse_wy - cy, mouse_wx - cx);

        if(!transition->is_loopback() && !angle_within(theta, a0, a1, dir == NVG_CCW))
            return TB_NONE;
        
        return TB_MID;
    }

    float t, d;
    distance_to_line(transition->x0, transition->y0, transition->x1, transition->y1,
            mouse_wx, mouse_wy, t, d);

    return t > 0 && t < 1 && abs(d) < LD_THRESH ? TB_MID : TB_NONE;
}
