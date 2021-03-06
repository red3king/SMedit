#pragma once

#define _USE_MATH_DEFINES   // for pi
#include <cmath>
#include <string>
#include <gtkmm.h>

#include "net/action.h"


using std::string;

// file stuff
bool file_to_string(string filename, string& output);
string make_relative_path(string project_folder, string path);  // return path, relative to project_folder. assumes path is contained within project_folder.
string make_absolute_path(string project_folder, string path);

// Return true if point within box
bool point_in_box(float pt_x, float pt_y, float box_x, float box_y, float box_w, float box_h);

// Return point moving from start point in specified direction (theta) and distance (r)
void move_in_direction(float x0, float y0, float r, float theta, float& x1, float& y1);

// Return distance d to line defined by x0,y0 and x1,y1.
// t is the value from the following equation for the point on the line closest to the given point:
// (x,y) = point0 + t*(point1 - point0)
void distance_to_line(float x0, float y0, float x1, float y1, float ptx, float pty, float& t, float& d);


float distance(float x0, float y0, float x1, float y1);

// clip angle to between 0 and 2pi
void clip_angle(float& theta);

// return true if theta is between a0 and a1
bool angle_within(float theta, float a0, float a1, bool counterclockwise);


void print_angle(float angle);


class IOResult
{
    public:
        IOResult(bool success, string fail_msg);
        IOResult(bool success);

        bool success;
        string fail_msg;

    private:

};


string int_to_hex_string(int input);


// String to number helpers
// returns true when input is valid
bool is_number(Glib::ustring text, bool allow_float=true);
bool string_to_int(int& result, string input);
bool string_to_float(float& result, string input);


// UI Helpers
extern Gtk::Window* main_window;
void display_error(string message);
void display_action_error(Action* failed_action);

// add value types to combo box
void prepare_value_type_combobox(Gtk::ComboBoxText* combo_box);
void set_margins(Gtk::Widget* widget, int margins);
void left_align(Gtk::Widget* widget);
void right_align(Gtk::Widget* widget);
void center_align(Gtk::Widget* widget);





template<typename T, typename A>
bool vector_contains(std::vector<T,A> const& vector, T element) 
{
    for(int i = 0; i < vector.size(); i++)
        if(vector[i] == element)
            return true;
        
    return false;
}

