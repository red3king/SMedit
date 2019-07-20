#include "gui_transition.h"


GMTransition::GMTransition(DrawContext* ctx, Transition* transition) : GUIModel(ctx, TRANSITION)
{
    this->transition = transition;
}


void GMTransition::draw()
{

}


CursorType GMTransition::update(CurrentEvents& current_events)
{
    return CT_DEFAULT;
}


Entity* GMTransition::get_entity()
{
    return transition;
}
