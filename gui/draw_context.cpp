#include "draw_context.h"


#define ZFAC 1.2


DrawContext::DrawContext(Gtk::GLArea* gl_area)
{
    this->gl_area = gl_area;
    reset();
}


Gtk::GLArea* DrawContext::get_gl_area()
{
    return gl_area;
}


void DrawContext::set_nvg_context(NVGcontext* vg)
{
    this->vg = vg;
    font_hack = nvgCreateFont(vg, "hack", "resources/hackfont/Hack-Regular.ttf");
    font_hack_bold = nvgCreateFont(vg, "hack bold", "resources/hackfont/Hack-Bold.ttf");

    // TODO check if these are zero -> load failed.
    icon_coding = nvgCreateImage(vg, "resources/icons/coding.png", 0);
    icon_flow = nvgCreateImage(vg, "resources/icons/flow.png", 0);
    icon_fork = nvgCreateImage(vg, "resources/icons/fork.png", 0);
    icon_join = nvgCreateImage(vg, "resources/icons/join.png", 0);
    icon_lock = nvgCreateImage(vg, "resources/icons/lock.png", 0);
    icon_uturn = nvgCreateImage(vg, "resources/icons/uturn.png", 0);

    notif_icon_lock = icon_lock;
    notif_icon_error = nvgCreateImage(vg, "resources/icons/error.png", 0);
}


void DrawContext::reset()
{
    woffset_x = 0;
    woffset_y = 0;
    zoom_factor = 1;
}


void DrawContext::world_to_screen(float& screen_x, float& screen_y, float world_x, float world_y)
{
    screen_x = zoom_factor * (world_x - woffset_x);
    screen_y = zoom_factor * (world_y - woffset_y);
}


float DrawContext::world_dist_to_screen(float world_dist)
{
    return world_dist * zoom_factor;
}


void DrawContext::screen_to_world(float& world_x, float& world_y, float screen_x, float screen_y)
{
    world_x = (screen_x / zoom_factor) + woffset_x;
    world_y = (screen_y / zoom_factor) + woffset_y;
}


float DrawContext::screen_dist_to_world(float screen_dist)
{
    return screen_dist / zoom_factor;
}


void DrawContext::zoom(float zoom_amount, float fp_screen_x, float fp_screen_y)
{
    float wx1, wx2, wy1, wy2, dx, dy;

    screen_to_world(wx1, wy1, fp_screen_x, fp_screen_y);
    zoom_factor *= (1 + zoom_amount/10.0);
    screen_to_world(wx2, wy2, fp_screen_x, fp_screen_y);

    dx = wx2 - wx1;
    dy = wy2 - wy1;
    woffset_x -= dx;
    woffset_y -= dy;
}


void DrawContext::zoom_raw(float zoom_factor)
{
    this->zoom_factor = zoom_factor;
}


void DrawContext::move(float world_dx, float world_dy)
{
    woffset_x -= world_dx;
    woffset_y -= world_dy;
}


void DrawContext::move_raw(float world_x, float world_y)
{
    woffset_x = world_x;
    woffset_y = world_y;
}


void DrawContext::draw_text_noclip(std::string text, int font, float size, NVGcolor color, float x, float y)
{
    nvgFontSize(vg, size);
    nvgFontFaceId(vg, font);
    nvgFillColor(vg, color);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, x, y, text.c_str(), nullptr);
}


void DrawContext::draw_text_one_line(std::string text, int font, float size, NVGcolor color, float x, float y,
        float max_width)
{
    nvgTextBreakLines(vg, text.c_str(), nullptr, max_width, &one_line_row, 1);

    nvgFontSize(vg, size);
    nvgFontFaceId(vg, font);
    nvgFillColor(vg, color);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, x, y, one_line_row.start, one_line_row.end);   
}


void DrawContext::measure_text(float& width, float& height, std::string text, int font, float size, float max_width)
{
    float bounds[4];
    
    if(max_width < 0)
        nvgTextBounds(vg, 0, 0, text.c_str(), nullptr, bounds);

    else
        nvgTextBoxBounds(vg, 0, 0, max_width, text.c_str(), nullptr, bounds);

    width = bounds[2] - bounds[0];
    height = bounds[3] - bounds[1];
}


void DrawContext::draw_text_many_lines(std::string text, int font, float size, NVGcolor color, float x,
        float y, float max_width, float max_height)
{
    nvgFontSize(vg, size);
    nvgFontFaceId(vg, font);
    nvgFillColor(vg, color);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgTextBox(vg, x, y, max_width, text.c_str(), nullptr);

}


float DrawContext::get_zoom_factor()
{
    return zoom_factor;
}


void DrawContext::get_offsets(float& woffset_x, float& woffset_y)
{
    woffset_x = this->woffset_x;
    woffset_y = this->woffset_y;
}


void DrawContext::get_screen_size(int &screen_x, int &screen_y)
{
   screen_x = gl_area->get_width();
   screen_y = gl_area->get_height();
}
