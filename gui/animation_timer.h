#pragma once

#include <boost/function.hpp>
#include <gtkmm.h>
#include <vector>
#include <map>

#include "gui/gui_context.h"

using std::vector;
using std::map;


#define ANIMATION_TICK_MS 33    // 30 fps, but event loop probably runs around 15 fps or something


class AnimationTimer
{
    public:
        AnimationTimer(GUIContext* gui_context);

        int add_request();
        void remove_request(int submission_id);

    private:
        GUIContext* gui_context;
        vector<int> requests;

        sigc::connection timer_connection;
        int _id_ctr;

        bool on_tick();
};



extern AnimationTimer *animation_timer;
