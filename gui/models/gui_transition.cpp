#include "gui/colors.h"
#include "gui_transition.h"
#include "utils.h"


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

    interp(.05, d0x, d0y);
    interp(.95, d1x, d1y);
    interp(.1, lsx, lsy);
    interp(.9, lex, ley);
    interp(.7, apx, apy);

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
    nvgStrokeColor(vg, GRAY);
    nvgStrokeWidth(vg, 2.5);
    nvgStroke(vg);

    // Draw dots
    nvgReset(vg);
    nvgBeginPath(vg);
    nvgCircle(vg, d0x, d0y, 4);
    nvgCircle(vg, d1x, d1y, 4);
    nvgFillColor(vg, GRAY);
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
    return false;
}
