#include "current_events.h"

#include <iostream>


MouseButton get_button(guint button)
{
    switch(button)
    {
        case 1:
            return MB_LEFT;
        case 2:
            return MB_MIDDLE;
        case 3:
            return MB_RIGHT;
        default:
            return MB_NONE;
    }
}


Key get_key(guint keyval)
{
    // see https://gitlab.gnome.org/GNOME/gtk/raw/master/gdk/gdkkeysyms.h
    switch(keyval)
    {
        // bug if user is silly enough to use both ctrls at once
        // but who would do that
        case GDK_KEY_Control_L:
        case GDK_KEY_Control_R:
            return K_CTRL;
        case GDK_KEY_s:
        case GDK_KEY_S:
            return K_S;
        case GDK_KEY_t:
        case GDK_KEY_T:
            return K_T;
        case GDK_KEY_l:
        case GDK_KEY_L:
            return K_L;
        default:
            return K_NONE;
    }
}


CurrentEvents::CurrentEvents()
{
    clear_current_event();
}


void CurrentEvents::clear_current_event()
{
    event_type = ET_NONE;
    key = K_NONE;
    mouse_button = MB_NONE;
    mouse_scroll = SC_NONE;
    scroll_amount = 0;

    ap_target_x = 0;
    ap_target_y = 0;
    ap_target_zoom = 1;
    is_ap = false;
}


void CurrentEvents::enable_ap(float ap_initial_x, float ap_initial_y, float ap_initial_zoom,
        float ap_target_x, float ap_target_y, float ap_target_zoom)
{
    is_ap = true;
    ap_when = high_resolution_clock::now();

    this->ap_initial_x = ap_initial_x;
    this->ap_initial_y = ap_initial_y;
    this->ap_initial_zoom = ap_initial_zoom;

    this->ap_target_x = ap_target_x;
    this->ap_target_y = ap_target_y;
    this->ap_target_zoom = ap_target_zoom;
}


void CurrentEvents::disable_ap()
{
    is_ap = false;
}


bool CurrentEvents::key_pressed(Key key)
{
    return pressed_keys.find(key) != pressed_keys.end();
}


bool CurrentEvents::button_pressed(MouseButton button)
{
    return pressed_buttons.find(button) != pressed_buttons.end();
}


void CurrentEvents::on_button_event(GdkEventButton* button_event)
{
    mouse_x = (float) button_event->x;
    mouse_y = (float) button_event->y;
    mouse_button = get_button(button_event->button);

    if(button_event->type == GDK_BUTTON_PRESS)
    {
        pressed_buttons.insert(mouse_button);
        event_type = ET_MB_PRESS;
    }
    else if(button_event->type == GDK_BUTTON_RELEASE)
    {
        pressed_buttons.erase(mouse_button);
        event_type = ET_MB_RELEASE;
    }
    else if(button_event->type == GDK_2BUTTON_PRESS)
    {
        event_type = ET_MB_DOUBLE;
    }
    else if(button_event->type == GDK_3BUTTON_PRESS)
    {   
        event_type = ET_MB_TRIPLE;
    }
}


void CurrentEvents::on_scroll_event(GdkEventScroll* scroll_event)
{
    if(scroll_event->direction == GDK_SCROLL_LEFT || scroll_event->direction == GDK_SCROLL_RIGHT)
        return;

    event_type = ET_SCROLL;
    mouse_x = (float) scroll_event->x;
    mouse_y = (float) scroll_event->y;

    if(scroll_event->direction == GDK_SCROLL_DOWN)
    {
        mouse_scroll = SC_DOWN;
        scroll_amount = -1.0;        
    }

    else if(scroll_event->direction == GDK_SCROLL_UP)
    {
        mouse_scroll = SC_UP;
        scroll_amount = 1.0;
    }

    else if(scroll_event->direction == GDK_SCROLL_SMOOTH)
    {
        double dx, dy;
        gdk_event_get_scroll_deltas((GdkEvent*)scroll_event, &dx, &dy);
        mouse_scroll = dy > 0 ? SC_UP : SC_DOWN;
        scroll_amount = dy;
    }
}


void CurrentEvents::on_key_event(GdkEventKey* key_event)
{
    key = get_key(key_event->keyval);

    if(key_event->type == GDK_KEY_PRESS)
    {
        pressed_keys.insert(key);
        event_type = ET_KEY_PRESS;
    }
    else
    {
        pressed_keys.erase(key);
        event_type = ET_KEY_RELEASE;
    }
}


void CurrentEvents::on_motion_notify_event(GdkEventMotion* motion_event)
{
    event_type = ET_M_MOVE;
    mouse_x = (float) motion_event->x;
    mouse_y = (float) motion_event->y;
}
