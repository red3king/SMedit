#include <string>

#include "utils.h"


bool point_in_box(int pt_x, int pt_y, int box_x, int box_y, int box_w, int box_h)
{
    return pt_x >= box_x && pt_x <= box_x + box_w &&
        pt_y >= box_y && pt_y <= box_y + box_h;
}


IOResult::IOResult(bool success, string fail_msg)
{
    this->success = success;
    this->fail_msg = fail_msg;
}
