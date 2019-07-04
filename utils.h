#pragma once

#include <string>

using std::string;


bool point_in_box(int pt_x, int pt_y, int box_x, int box_y, int box_w, int box_h);


class IOResult
{
    public:
        IOResult(bool success, string fail_msg);

        bool success;
        string fail_msg;

    private:

};
