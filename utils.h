#pragma once

#define _USE_MATH_DEFINES   // for pi
#include <cmath>
#include <string>

using std::string;


bool point_in_box(float pt_x, float pt_y, float box_x, float box_y, float box_w, float box_h);


class IOResult
{
    public:
        IOResult(bool success, string fail_msg);

        bool success;
        string fail_msg;

    private:

};


void move_in_direction(float x0, float y0, float r, float theta, float& x1, float& y1);

