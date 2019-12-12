#include <boost/bind.hpp>
#include <chrono>

#include "log.h"
#include "auto_pan.h"

using namespace std::chrono;


AutoPan::AutoPan(GUIState& gui_state)
{
    has_last_update = false;
    animation_callback = boost::bind(&GUIState::force_gui_context_update, &gui_state);
    cb_id = animation_timer.add_callback(animation_callback);
}


bool AutoPan::maybe_create(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(current_events.is_ap)
    {
        pref = new AutoPan(gui_state);
        return true;
    }

    return false;
}


GUIOpResult AutoPan::should_continue(GUIState& gui_state, CurrentEvents& current_events)
{
    float dt = get_elapsed_seconds(current_events);
    float t = dt / AUTO_PAN_SECONDS;

    if(t > 1)
        return END;
    
    return CONTINUE;
}


double AutoPan::get_elapsed_seconds(CurrentEvents& current_events)
{
    millitime now = high_resolution_clock::now();
    millitime then = current_events.ap_when;
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count() / 1000.0;
}


bool AutoPan::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    CurrentEvents& c = current_events;
    float dt = get_elapsed_seconds(current_events);
    float t = dt / AUTO_PAN_SECONDS;   // t is parameter from 0 to 1

    //  f(x) = x0 + t(x1 - x0) linear interpolate position
    float new_x = c.ap_initial_x + t * (c.ap_target_x - c.ap_initial_x);
    float new_y = c.ap_initial_y + t * (c.ap_target_y - c.ap_initial_y);

    /*
     * When we move the world offset linearly in time and move the zoom factor linearly in time,
     * this turns out not to be a smooth zoom/pan. 
     *
     * Instead, math was done to ensure that the path of all screen coordinate pairs remain lines
     * as the zoom factor changes. That math results in the following calculation:
    */

    float z0_inv = 1.0 / c.ap_initial_zoom;
    float z1_inv = 1.0 / c.ap_target_zoom;
    float new_z = 1.0 / (z0_inv + t * (z1_inv - z0_inv)); 
    
    float old_x, old_y;
    gui_state.draw_context.get_offsets(old_x, old_y);

    gui_state.draw_context.move_raw(new_x, new_y);
    gui_state.draw_context.zoom_raw(new_z);
    return false;
}


bool AutoPan::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op)
{
    current_events.disable_ap();
    animation_timer.remove_callback(cb_id);
    return false;
}
