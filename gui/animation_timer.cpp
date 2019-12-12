#include "animation_timer.h"
#include "log.h"


AnimationTimer::AnimationTimer()
{
    _id_ctr = 0;
}


int AnimationTimer::add_callback(AnimationCB callback)
{
    int submission_id = _id_ctr++;

    bool started = callbacks.size() > 0;
    callbacks[submission_id] = callback;

    //start if not
    if(!started)
        timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimationTimer::on_tick), ANIMATION_TICK_MS);

    return submission_id;
}


void AnimationTimer::remove_callback(int submission_id)
{
    callbacks.erase(submission_id);

    if(callbacks.size() == 0)
        timer_connection.disconnect();
}


bool AnimationTimer::on_tick()
{
    for(auto it=callbacks.begin(); it != callbacks.end(); it++)
        it->second();

    return true;
}


AnimationTimer animation_timer;
