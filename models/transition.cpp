#include "transition.h"


Transition::Transition(unsigned int id) : Entity(id) 
{ 
    from_state = nullptr;
    to_state = nullptr;
    type = EVENT;
    event_name = "";
    x0 = 0;
    y0 = 0;
    x1 = 0;
    y1 = 0;
}


bool Transition::from_connected()
{
    return from_state != nullptr;
}


bool Transition::to_connected()
{
    return to_state != nullptr;
}


bool Transition::any_connected()
{
    return from_connected() || to_connected();
}

