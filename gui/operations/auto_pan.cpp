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
    //log("auto pan on_continue()");
    CurrentEvents& c = current_events;
    float dt = get_elapsed_seconds(current_events);
    float t = dt / AUTO_PAN_SECONDS;   // t is parameter from 0 to 1

    //  f(x) = x0 + t(x1 - x0) linear interp on position
    float new_x = c.ap_initial_x + t * (c.ap_target_x - c.ap_initial_x);
    float new_y = c.ap_initial_y + t * (c.ap_target_y - c.ap_initial_y);


    // some exponential stuff so the thing doesnt spiral
    float new_z = c.ap_initial_zoom + t*t * (c.ap_target_zoom - c.ap_initial_zoom);
    
    
    log("\tt=" + std::to_string(t));
    float old_x, old_y;
    gui_state.draw_context.get_offsets(old_x, old_y);
    log("\told_x=" + std::to_string(old_x));
    log("\told_y=" + std::to_string(old_y));
    log("\tnew_x=" + std::to_string(new_x));
    log("\tnew_y=" + std::to_string(new_y));
    
    //log("\tnew_z=" + std::to_string(new_z));
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
