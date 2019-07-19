#include "move_resourcelock.h"
#include "historymanager/operations/resourcelock_ops.h"


MoveResourceLock::MoveResourceLock(GMResourceLock* rlock_gmodel, float offset_x, float offset_y)
{
    this->offset_x = offset_x;
    this->offset_y = offset_y;
    this->rlock_gmodel = rlock_gmodel;
}


bool MoveResourceLock::maybe_create(GMResourceLock* rlock_gmodel, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type != ET_MB_PRESS)
        return false;
    
    float x, y, w, h;
    rlock_gmodel->get_coords(x, y, w, h);

    if(rlock_gmodel->mouse_within(current_events.mouse_x, current_events.mouse_y))
    {
        float mouse_x_world, mouse_y_world;
        gui_state.draw_context.screen_to_world(mouse_x_world, mouse_y_world, current_events.mouse_x, current_events.mouse_y);
        float offset_x = mouse_x_world - x;
        float offset_y = mouse_y_world - y;
        pref = new MoveResourceLock(rlock_gmodel, offset_x, offset_y);
        return true;
    }

    return false;
}


GUIOpResult MoveResourceLock::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    if(!current_events.button_pressed(MB_LEFT))
        return END;

    return CONTINUE;
}


bool MoveResourceLock::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float mouse_x_world, mouse_y_world;
    gui_state.draw_context.screen_to_world(mouse_x_world, mouse_y_world, current_events.mouse_x, current_events.mouse_y);
    float new_x = mouse_x_world - offset_x;
    float new_y = mouse_y_world - offset_y;
    op = new OpResourceLockMove(gui_state.current_machine, rlock_gmodel->resourcelock, new_x, new_y);
    return true;
}



