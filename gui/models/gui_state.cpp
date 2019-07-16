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


void GMState::draw_interior()
{
    
}
