#pragma once

#define _USE_MATH_DEFINES   // for pi
#include <cmath>
#include <string>

using std::string;


// Return true if point within box
bool point_in_box(float pt_x, float pt_y, float box_x, float box_y, float box_w, float box_h);

// Return point moving from start point in specified direction (theta) and distance (r)
void move_in_direction(float x0, float y0, float r, float theta, float& x1, float& y1);

// Return distance d to line defined by x0,y0 and x1,y1.
// t is the value from the following equation for the point on the line closest to the given point:
// (x,y) = point0 + t*(point1 - point0)
void distance_to_line(float x0, float y0, float x1, float y1, float ptx, float pty, float& t, float& d);


float distance(float x0, float y0, float x1, float y1);


class IOResult
{
    public:
        IOResult(bool success, string fail_msg);

        bool success;
        string fail_msg;

    private:

};



