#pragma once

#include <boost/function.hpp>
#include <gtkmm.h>
#include <vector>
#include <map>

using std::vector;
using std::map;
typedef boost::function<void()> AnimationCB;


#define ANIMATION_TICK_MS 35


class AnimationTimer
{
    public:
        AnimationTimer();

        int add_callback(AnimationCB callback);
        void remove_callback(int submission_id);

    private:
        map<int, AnimationCB> callbacks;
        
        sigc::connection timer_connection;
        int _id_ctr;

        bool on_tick();
};



extern AnimationTimer animation_timer;
