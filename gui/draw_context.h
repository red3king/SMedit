#pragma once

#include <string>

#include <gtkmm.h>
#include "lib/nanovg/nanovg.h"


class DrawContext
{
    public:
        DrawContext(Gtk::GLArea* gl_area);
        void set_nvg_context(NVGcontext* vg);
        
        // transforms between screen coordinates in pixels and the world
        void world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y); 
        float world_dist_to_screen(float world_dist);
        void screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y);
        float screen_dist_to_world(float screen_dist);

        void move(float world_dx, float world_dy);  // move viewport, in world coords
        void zoom(float zoom_amount, float fp_screen_x, float fp_screen_y);  // zoom in/out from fixed point

        void move_raw(float world_x, float world_y);
        void zoom_raw(float zoom_factor);   // straight up set zoom factor

        void reset();

        NVGcontext* vg;

        NVGtextRow one_line_row;

        float measure_text(std::string text, int font, float size);
        void draw_text_noclip(std::string text, int font, float size, NVGcolor color, float x, float y);
        void draw_text_one_line(std::string text, int font, float size, NVGcolor color, float x, float y,
                float max_width);
        void draw_text_many_lines(std::string text, int font, float size, NVGcolor color, float x, float y,
                float max_width, float max_height);

        // resources
        int font_hack, font_hack_bold;
        int icon_coding, icon_flow, icon_fork, icon_join, icon_lock,
            icon_uturn;

        float get_zoom_factor();
        void get_offsets(float& woffset_x, float& woffset_y);

        void get_screen_size(int &screen_x, int &screen_y);

        Gtk::GLArea* get_gl_area();

        float zoom_factor, woffset_x, woffset_y;
    private:
        Gtk::GLArea* gl_area;  // to get screen size
};
