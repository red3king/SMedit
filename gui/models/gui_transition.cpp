#include "gui_transition.h"


GMTransition::GMTransition(DrawContext* ctx, Transition* transition) : GUIModel(ctx, TRANSITION)
{
    this->transition = transition;
}


void GMTransition::draw()
{

}


bool GMTransition::mouse_in_range(float mouse_x, float mouse_y)
{
    return false;
}


Entity* GMTransition::get_entity()
{
    return transition;
}
