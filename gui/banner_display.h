#pragma once

#include <string>

#include "timestuff.h"
#include "draw_context.h"

using std::string;

#define BANNER_DISP_SOLID_MS 1000
#define BANNER_DISP_FADE_MS  1000
#define BANNER_DISP_TOTAL_MS BANNER_DISP_SOLID_MS + BANNER_DISP_FADE_MS


class BannerDisplayer
{
    public:
        BannerDisplayer();

        void display_banner(string message);

        void update();
        void draw(DrawContext& draw_context);

    private:
        int animation_timer_request;
        int fade_alpha;
        bool active;
        string current_message;
        millitime start_time;
};

