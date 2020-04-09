#include "transition.h"
#include "utils.h"

#include <sstream>
#include <iomanip>


Transition::Transition(unsigned int id) : Entity(id) 
{ 
    from_state = nullptr;
    to_state = nullptr;
    type = EVENT;
    loopback_position = CP_NONE;
    event_name = "";
    x0 = 0;
    y0 = 0;
    x1 = 0;
    y1 = 0;
    timeout = 0;
    is_child_transition = false;
}


Transition::Transition(const Transition &other) : Entity(other)
{
    // defining to keep pointer null
    type = other.type;
    event_name = other.event_name;
    timeout = other.timeout;

    from_state = nullptr;
    to_state = nullptr;

    x0 = other.x0;
    x1 = other.x1;
    y0 = other.y0;
    y1 = other.y1;

    loopback_position = other.loopback_position;
    is_child_transition = other.is_child_transition;
}


Transition::Transition(json jdata) : Entity(jdata)
{
    from_state = nullptr;
    to_state = nullptr;
    loopback_position = CP_NONE;

    type = (TransitionType) jdata["type"];
    event_name = jdata["event_name"];
    timeout = jdata["timeout"];
    x0 = jdata["x0"];
    x1 = jdata["x1"];
    y0 = jdata["y0"];
    y1 = jdata["y1"];
    is_child_transition = jdata["is_child_transition"];
}


json Transition::to_json()
{
    json jdata = Entity::to_json();
    jdata["type"] = (int)type;
    jdata["event_name"] = event_name;
    jdata["timeout"] = timeout;
    jdata["x0"] = x0;
    jdata["x1"] = x1;
    jdata["y0"] = y0;
    jdata["y1"] = y1;
    jdata["from_state"] = from_state == nullptr ? 0 : from_state->id;
    jdata["to_state"] = to_state == nullptr ? 0 : to_state->id;
    jdata["is_child_transition"] = is_child_transition;
    return jdata;
}


EntityType Transition::get_entity_type()
{
    return TRANSITION;
}


float Transition::get_length()
{
    return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}


string Transition::describe()
{
    if(type == EVENT)
        return "ev: " + event_name;

    if(type == TIMEOUT)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << "timeout (" << timeout << ")";
        return stream.str();
    }

    if(type == CATCHALL)
        return "*";
    
    return "oops";
}


bool Transition::from_connected()
{
    return from_state != nullptr;
}


bool Transition::to_connected()
{
    return to_state != nullptr;
}


bool Transition::is_loopback()
{
    return to_state == from_state && to_state != nullptr;
}


bool Transition::any_connected()
{
    return from_connected() || to_connected();
}


void Transition::update_positions()
{
    // Recalculate dependant transition endpoint positions
    // Happens when a State moves or user moves free end of 
    // connected Transition.

    bool _is_loopback = false;

    if(from_connected() && to_connected())
    {
        if(is_loopback())
        {
            _is_loopback = true;
            update_loopback();
        }
        else
            update_two_positions();
    }

    else if(from_connected())
        update_single_position(from_state, x0, y0, x1, y1);

    else if(to_connected())
        update_single_position(to_state, x1, y1, x0, y0);

    if(!_is_loopback)
        loopback_position = CP_NONE;
}


void Transition::update_loopback()
{
    float xposes[] = { from_state->x, from_state->x + from_state->w, from_state->x, from_state->x + from_state->w };
    float yposes[] = { from_state->y, from_state->y, from_state->y + from_state->h, from_state->y + from_state->h };

    if(loopback_position == CP_NONE)
    {
        float min_dist = 100000;
        int min_index = -1;
        float x = (x1 + x0) / 2.0;
        float y = (y1 + y0) / 2.0;

        for(int i=0; i<4; i++)
        {
            float dist = distance(x, y, xposes[i], yposes[i]);
            if(dist < min_dist)
            {
                min_dist = dist;
                min_index = i;
            }
        }

        loopback_position = (CornerPosition)min_index;
    }

    x0 = xposes[loopback_position];
    y0 = yposes[loopback_position];
    x1 = xposes[loopback_position];
    y1 = yposes[loopback_position];
}


void _clip_to_bounds(float& val, float min, float max)
{
    if(val < min)
        val = min;

    else if(val > max)
        val = max;
}   


void _adjust_bounds(float& from_val, float& to_val, float from_pos, float from_size, float to_pos, float to_size)
{
    float min, max;
    // get bounds   
    if(from_pos < to_pos)
    {
        max = from_pos + from_size;
        min = to_pos;
    }

    else
    {
        max = to_pos + to_size;
        min = from_pos;
    }

    // clip to bounds
    _clip_to_bounds(from_val, min, max);
    _clip_to_bounds(to_val, min, max);
}


void Transition::update_two_positions()
{
    bool is_above = to_state->y + to_state->h < from_state->y;
    bool is_below = to_state->y > from_state->y + from_state->h;
    bool is_leftof = to_state->x + to_state->w < from_state->x;
    bool is_rightof = to_state->x > from_state->x + from_state->w;

    // top
    if(is_above && !is_leftof && !is_rightof)
    {
        y0 = from_state->y;
        y1 = to_state->y + to_state->h;
        _adjust_bounds(x0, x1, from_state->x, from_state->w, to_state->x, to_state->w);
    }

    // top-right
    else if(is_above && is_rightof)
    {
        x0 = from_state->x + from_state->w;
        y0 = from_state->y;
        x1 = to_state->x;
        y1 = to_state->y + to_state->h;
    }
    
    // right    
    else if(is_rightof && !is_above && !is_below)
    {
        x0 = from_state->x + from_state->w;
        x1 = to_state->x;
        _adjust_bounds(y0, y1, from_state->y, from_state->h, to_state->y, to_state->h);
    }

    // bottom-right
    else if(is_below && is_rightof)
    {
        x0 = from_state->x + from_state->w;
        y0 = from_state->y + from_state->h;
        x1 = to_state->x;
        y1 = to_state->y;
    }

    // bottom
    else if(is_below && !is_rightof && !is_leftof)
    {
        y0 = from_state->y + from_state->h;
        y1 = to_state->y;
        _adjust_bounds(x0, x1, from_state->x, from_state->w, to_state->x, to_state->w);
    }
    
    // bottom-left
    else if(is_below && is_leftof)
    {
        x0 = from_state->x;
        y0 = from_state->y + from_state->h;
        x1 = to_state->x + to_state->w;
        y1 = to_state->y;
    }

    // left
    else if(is_leftof && !is_below && !is_above)
    {
        x0 = from_state->x;
        x1 = to_state->x + to_state->w;
        _adjust_bounds(y0, y1, from_state->y, from_state->h, to_state->y, to_state->h);
    }

    // top-left
    else if(is_leftof && is_above)
    {
        x0 = from_state->x;
        y0 = from_state->y;
        x1 = to_state->x + to_state->w;
        y1 = to_state->y + to_state->h;
    }
}


void Transition::update_single_position(State* connected_state, float& connected_x, float& connected_y, float& free_x, float free_y)
{
    bool is_above = free_y < connected_state->y;
    bool is_below = free_y > connected_state->y + connected_state->h;
    bool is_rightof = free_x > connected_state->x + connected_state->w;
    bool is_leftof = free_x < connected_state->x; 
   
    // top 
    if(is_above && !is_rightof && !is_leftof)
    {
        connected_y = connected_state->y;
        connected_x = free_x;
    }    
       
    // top-right
    else if(is_above && is_rightof)
    {
        connected_y = connected_state->y;
        connected_x = connected_state->x + connected_state->w;
    }
    
    // right
    else if(is_rightof && !is_above && !is_below)
    {
        connected_y = free_y;
        connected_x = connected_state->x + connected_state->w;
    }   

    // bottom-right
    else if(is_rightof && is_below)
    {
        connected_y = connected_state->y + connected_state->h;
        connected_x = connected_state->x + connected_state->w;
    }

    // bottom
    else if(is_below && !is_rightof && !is_leftof)
    {
        connected_y = connected_state->y + connected_state->h;
        connected_x = free_x;
    }

    // bottom-left
    else if(is_below && is_leftof)
    {
        connected_y = connected_state->y + connected_state->h;
        connected_x = connected_state->x;
    }
    
    // left
    else if(is_leftof && !is_below && !is_above)
    {
        connected_y = free_y;
        connected_x = connected_state->x;
    }

    // top-left
    else if(is_leftof && is_above)
    {
        connected_y = connected_state->y;
        connected_x = connected_state->x;
    }
}
