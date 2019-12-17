#include "animation_timer.h"
#include "log.h"


AnimationTimer::AnimationTimer(GUIContext* gui_context)
{
    _id_ctr = 0;
    this->gui_context = gui_context;
}


int AnimationTimer::add_request()
{
    int submission_id = _id_ctr++;
    bool started = requests.size() > 0;
    requests.push_back(submission_id);

    //start if not
    if(!started)
        timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimationTimer::on_tick), ANIMATION_TICK_MS);

    return submission_id;
}


void AnimationTimer::remove_request(int submission_id)
{
    int i=0;
    for(; i<requests.size(); i++)
    {
        if(requests[i] == submission_id)
            break;
    }

    requests.erase(requests.begin() + i); 

    if(requests.size() == 0)
        timer_connection.disconnect();
}


bool AnimationTimer::on_tick()
{
    gui_context->update();
    return true;
}


AnimationTimer *animation_timer;
