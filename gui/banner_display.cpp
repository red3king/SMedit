#include "banner_display.h"
#include "animation_timer.h"
#include "gui/colors.h"


BannerDisplayer::BannerDisplayer()
{
    animation_timer_request = 0;
    active = false;
    fade_alpha = 0;
}


void BannerDisplayer::display_banner(string message)
{
    current_message = message;
    start_time = high_resolution_clock::now();

    if(!active)
    {
        animation_timer_request = animation_timer->add_request();
        active = true;
    }    
}


void BannerDisplayer::update()
{
    if(!active)
        return;

    // check if done
    millitime now = high_resolution_clock::now();
    int elapsed_ms = timedelta_ms(now, start_time);

    if(elapsed_ms > BANNER_DISP_TOTAL_MS)
    {
        active = false;
        animation_timer->remove_request(animation_timer_request);
    }
    
    else if(elapsed_ms < BANNER_DISP_SOLID_MS)
        fade_alpha = 255;

    else if(elapsed_ms < BANNER_DISP_TOTAL_MS)
        fade_alpha = 255 * (BANNER_DISP_FADE_MS - (elapsed_ms - BANNER_DISP_SOLID_MS)) / BANNER_DISP_FADE_MS;

}


void BannerDisplayer::draw(DrawContext& draw_context)
{
    if(!active)
        return;
    
    auto color = nvgRGBA(255, 255, 255, fade_alpha);
    draw_context.draw_text_noclip(current_message, draw_context.font_hack_bold, 38.0, color, 15, 25);
}
