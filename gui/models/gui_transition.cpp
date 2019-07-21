#include "gui_transition.h"


GMTransition::GMTransition(DrawContext* ctx, Transition* transition) : GUIModel(ctx, TRANSITION)
{
    this->transition = transition;
}


void GMTransition::draw()
{

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
