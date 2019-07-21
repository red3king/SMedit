#include "gui_state.h"


GMState::GMState(DrawContext* ctx, State* state) : GMBox(ctx, STATE)
{
    this->state = state;
}


void GMState::get_coords(float&x, float&y, float&w, float&h)
{
    x = state->x;
    y = state->y;
    w = state->w;
    h = state->h;
}


void GMState::set_coords(float x, float y, float w, float h)
{
    state->x = x;
    state->y = y;
    state->w = w;
    state->h = h;
}


Entity* GMState::get_entity()
{
    return state;
}


NVGcolor GMState::get_border_color()
{
    return nvgRGBA(255, 255, 255, 255);
}


NVGcolor GMState::get_title_text_color()
{
    return nvgRGBA(0, 0, 0, 255);
}


std::string GMState::get_title()
{
    return state->name;
}


void GMState::draw_interior(float x, float y, float w, float h)
{
    NVGcontext* vg = ctx->vg;
    nvgBeginPath(vg);     
    nvgRect(vg, x, y, w, h);
    nvgFillColor(vg, nvgRGBA(0x00, 0x2b, 0x36, 155));
    nvgFill(vg);
}


