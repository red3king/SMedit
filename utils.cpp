
#include "utils.h"


bool point_in_box(float pt_x, float pt_y, float box_x, float box_y, float box_w, float box_h)
{
    return pt_x >= box_x && pt_x <= box_x + box_w &&
        pt_y >= box_y && pt_y <= box_y + box_h;
}


void move_in_direction(float x0, float y0, float r, float theta, float& x1, float& y1)
{
    x1 = x0 + r * cos(theta);
    y1 = y0 + r * sin(theta);
}


void distance_to_line(float x0, float y0, float x1, float y1, float ptx, float pty, float& t, float& d)
{
    float theta = atan2(y1 - y0, x1 - x0);
    float theta_p = atan2(pty - y0, ptx - x0);
    float d_theta = theta_p - theta;
    float hyp = distance(x0, y0, ptx, pty);
    d = hyp * sin(d_theta);
    float adj = hyp * cos(d_theta);
    float segdist  = distance(x0, y0, x1, y1);
    t = adj / segdist;
}


float distance(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}


IOResult::IOResult(bool success, string fail_msg)
{
    this->success = success;
    this->fail_msg = fail_msg;
}
