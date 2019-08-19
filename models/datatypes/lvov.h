#pragma once

#include <string>
#include <gtkmm.h>

#include "utils.h"

using std::string;


#define SVT_ANY "any"
#define SVT_STR "string"
#define SVT_INT "int"
#define SVT_FLOAT "float"
#define SVT_REF "reference"


enum ValueType { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };
static const ValueType AllValueTypes[] = { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };


string value_type_to_string(ValueType vt);
ValueType string_to_value_type(string str);


class LVOV
{
    // Literal Value or Variable name
    public:
        LVOV();
        LVOV(string str_val);
        LVOV(string str_val, bool is_variable);
        LVOV(int int_val);
        LVOV(float float_val);

        void reset();

        bool from_string(Glib::ustring input); // attempt to update value from string

        // Returns string representation of value/varname 
        string to_string();

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

