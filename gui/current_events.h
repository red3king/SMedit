#pragma once

#include <unordered_set>
#include <gdk/gdk.h>

using std::unordered_set;


enum Key
{
    K_NONE,
    K_CTRL,
    K_S,
    K_T,
    K_L,
};


enum MouseButton
{
    MB_NONE,
    MB_LEFT, 
    MB_MIDDLE,
    MB_RIGHT,
};


enum MouseScroll
{
    SC_NONE,
    SC_UP,
    SC_DOWN
};


enum CurrentEventType
{
    ET_NONE,
    ET_KEY_PRESS,
    ET_KEY_RELEASE,
    ET_M_MOVE,
    ET_MB_PRESS,
    ET_MB_RELEASE,
    ET_MB_DOUBLE,
    ET_MB_TRIPLE,
    ET_SCROLL
};


class CurrentEvents
{
    //TODO

    // holds current set of keys held down, click events just made by user,
    // etc

    /*
     * types of thing
     *      left click tap      ->      select thing
     *      left click drag     ->      move thing, pan, resize thing, select things
     *      delete key tap      ->      deletes things
     *
     * */

    public:
        // relating to current event
        CurrentEventType event_type;
        Key key;
        MouseButton mouse_button;
        MouseScroll mouse_scroll;
        float scroll_amount;

        // stuff calculated from past events
        float mouse_x, mouse_y;
        unordered_set<Key> pressed_keys;
        unordered_set<MouseButton> pressed_buttons;

        void clear_current_event();

        bool button_pressed(MouseButton button);
        bool key_pressed(Key key);

        // https://developer.gnome.org/gtkmm/stable/classGtk_1_1Widget.html#a4b64421cad754fbd49ae17cbfe4814d0
        void on_button_event(GdkEventButton* button_event);   // for press and release
        void on_scroll_event(GdkEventScroll* scroll_event);
        void on_key_event(GdkEventKey* key_event);
        void on_motion_notify_event(GdkEventMotion* motion_event);
};
