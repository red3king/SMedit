#include "resize_box_entity.h"
#include "log.h"


ResizeBoxEntity::ResizeBoxEntity(GMBox* gm_box, BorderType resize_border)
{
    this->resize_border = resize_border;
    this->gm_box = gm_box;
}


bool ResizeBoxEntity::maybe_create(GMBox* gm_box, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.event_type != ET_MB_PRESS || current_events.mouse_button != MB_LEFT)
        return false;

    BorderType border = gm_box->mouse_on_border(current_events.mouse_x, current_events.mouse_y);

    if(border == BT_NONE)
        return false;

    pref = new ResizeBoxEntity(gm_box, border);
    return true;
}


GUIOpResult ResizeBoxEntity::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    if(current_events.button_pressed(MB_LEFT))
        return CONTINUE;

    return END;
}


bool ResizeBoxEntity::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    float new_position, mouse_wx, mouse_wy;
    gui_state.draw_context.screen_to_world(mouse_wx, mouse_wy, current_events.mouse_x, current_events.mouse_y);

    if(resize_border == BT_TOP || resize_border == BT_BOTTOM)
        new_position = mouse_wy;
    else
        new_position = mouse_wx;

    // don't submit operation if new height/width is too small
    if(!gm_box->new_border_valid(resize_border, new_position))
        return false;

    BoxEntity* entity = dynamic_cast<BoxEntity*>(gm_box->get_entity());
    op = new OpBoxEntityResize(gui_state.current_machine, entity, resize_border, new_position);
    return true;
}
