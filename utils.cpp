#include <string>

#include "utils.h"


bool point_in_box(float pt_x, float pt_y, float box_x, float box_y, float box_w, float box_h)
{
    return pt_x >= box_x && pt_x <= box_x + box_w &&
        pt_y >= box_y && pt_y <= box_y + box_h;
}


IOResult::IOResult(bool success, string fail_msg)
{
    this->success = success;
    this->fail_msg = fail_msg;
}
