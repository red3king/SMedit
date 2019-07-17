#pragma once

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
