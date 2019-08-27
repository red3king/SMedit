#pragma once

#include "gui_model.h"
#include "gui_box_model.h"


class GMResourceLock : public GMBox
{
    public:
        GMResourceLock(DrawContext* ctx, ResourceLock* resourcelock);

        void draw_interior(float x, float y, float w, float h);
        Entity* get_entity();
        NVGcolor get_border_color();
        NVGcolor get_title_text_color();
        std::string get_title();
        bool get_icon(int& icon_image);

        ResourceLock* resourcelock;

};

