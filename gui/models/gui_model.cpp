#include "gui_model.h"


GUIModel::GUIModel(DrawContext* ctx, EntityType type)
{
    this->type = type;
    this->ctx = ctx;

    selected = false;
    mouse_over = false;
}


unsigned int GUIModel::get_id()
{
    return get_entity()->id;
}


GUIModel::~GUIModel(){ }


void GUIModel::set_deselected()
{
    selected = false;
}


CursorType GUIModel::update(CurrentEvents& current_events, GUIModel*& just_selected)
{
    mouse_over = mouse_within(current_events.mouse_x, current_events.mouse_y);

    if(mouse_over)
    {
        bool clicked = current_events.event_type == ET_MB_PRESS && current_events.button_pressed(MB_LEFT);

        // If something else has already been selected because they're overlapping
        // don't become selected.
        if(clicked && just_selected == nullptr)
        {
            selected = true;
            just_selected = this;
        }
    }

    return update_impl(current_events);
}
