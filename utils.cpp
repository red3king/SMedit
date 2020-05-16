#include "utils.h"
#include "const.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>


Gtk::Window* main_window;


// file stuff

bool file_to_string(string filename, string& output)
{   
    try 
    {
        std::ifstream input_stream(filename);
        output = std::string( (std::istreambuf_iterator<char>(input_stream) ),
                           (std::istreambuf_iterator<char>()    ) );
    }
    catch(...)
    {
        return false;
    }

    return true;
}


string make_relative_path(string project_folder, string path)
{
    // TODO actually do relative path
    // right now we just get the filename, assuming all projects have flat structure structure.
    return std::filesystem::path(path).filename();  
}


string make_absolute_path(string project_folder, string path)
{
    return project_folder + "/" + path; // TODO refactor when make_relative_path is actually properly implemented
}


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


IOResult::IOResult(bool success)
{
    this->success = success;
    fail_msg = "";
}


void clip_angle(float& theta)
{
    while(theta < 0)
        theta += 2.0 * M_PI;

    while(theta > 2.0 * M_PI)
        theta -= 2.0 * M_PI;
}


bool angle_within(float theta, float a0, float a1, bool counterclockwise)
{
    clip_angle(a0);
    clip_angle(a1);
    clip_angle(theta);

    bool order_incorrect = a1 < a0;    
    float max = std::max(a0, a1);
    float min = std::min(a0, a1);

    return  (theta > min && theta < max) != order_incorrect != counterclockwise;
}


void print_angle(float angle)
{
    std::cout << 360.0 * angle / (2.0 * M_PI);
}


string int_to_hex_string(int input)
{
    std::stringstream stream;
    stream << std::hex << input;
    return string(stream.str());
}


bool is_number(Glib::ustring text, bool allow_float)
{
    // return true if text consists of numbers and periods
    string allowed = allow_float ? "1234567890." : "1234567890";
    bool ok;

    for(int i = 0; i < text.size(); i++)
    {
        ok = false;

        for(int j = 0; j < allowed.size(); j++)
        {
            if(text.at(i) == allowed.at(j))
            {
                ok = true;
                break;
            }
        }

        if(!ok)
            return false;
    }

    return true;
}


bool string_to_int(int& result, string input)
{
    int res2;
    try { res2 = std::stoi(input); }
    catch(...) { return false; }
    
    if(is_number(input))
    {
        result = res2;
        return true;
    }

    return false;
}


bool string_to_float(float& result, string input)
{
    float res2;
    try { res2 = std::stof(input); }
    catch(...) { return false; }
    
    if(is_number(input))
    {
        result = res2;
        return true;
    }

    return false;
}


// UI Helpers

void display_error(string message)
{
    Gtk::MessageDialog dialog(*main_window, "Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
    dialog.set_secondary_text(message);
    dialog.run();
}


void display_action_error(Action* failed_action)
{
    string message = "Server action " + std::to_string(failed_action->id) + " failed:\r\n\r\n" + 
        failed_action->error_message;
    display_error(message);
}


void prepare_value_type_combobox(Gtk::ComboBoxText* combo_box)
{
    for(int i = 0; i < NUM_VALUE_TYPES; i++)
    {
        ValueType vt = AllValueTypes[i];
        string str = value_type_to_string(vt);
        combo_box->append(str);
    }
}


void set_margins(Gtk::Widget* widget, int margins)
{
    widget->set_margin_left(margins);
    widget->set_margin_top(margins);
    widget->set_margin_right(margins);
    widget->set_margin_bottom(margins);
}


void left_align(Gtk::Widget* widget)
{
    widget->set_halign(Gtk::ALIGN_START);
    widget->set_valign(Gtk::ALIGN_CENTER);
}


void right_align(Gtk::Widget* widget)
{
    widget->set_halign(Gtk::ALIGN_END);
    widget->set_valign(Gtk::ALIGN_CENTER);
}


void center_align(Gtk::Widget* widget)
{
    widget->set_halign(Gtk::ALIGN_CENTER);
    widget->set_valign(Gtk::ALIGN_CENTER);
}

