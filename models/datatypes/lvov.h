#pragma once

#include <string>
#include <gtkmm.h>
#include "lib/json.hpp"

#include "utils.h"
#include "const.h"

using std::string;
using nlohmann::json;


class LVOV
{
    // Literal Value or Variable name
    public:
        LVOV();
        LVOV(string str_val);
        LVOV(string str_val, bool is_variable);
        LVOV(int int_val);
        LVOV(float float_val);
        LVOV(json jdata);

        void reset();

        bool from_string(Glib::ustring input); // attempt to update value from string

        json to_json();

        // Returns string representation of value/varname 
        string to_string();

        // string representation + data type string
        string describe();

        ValueType type;

        string variable_name;
        string str_val;
        int int_val;
        float float_val;

        // These operators return the underlying value or variable name
        operator int() const;
        operator string() const;
        operator float() const;
};

