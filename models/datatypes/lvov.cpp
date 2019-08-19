#include "lvov.h"


string value_type_to_string(ValueType vt)
{
    switch(vt)
    {
        case VT_ANY:
            return SVT_ANY;
        case VT_STR:
            return SVT_STR;
        case VT_INT:
            return SVT_INT;
        case VT_FLOAT:
            return SVT_FLOAT;
        case VT_REF:
            return SVT_REF;
    }

    throw std::invalid_argument("unknown ValueType");
}


ValueType string_to_value_type(string str)
{

    if(str == SVT_ANY)
        return VT_ANY;

    if(str == SVT_STR)
        return VT_STR;

    if(str == SVT_INT)
        return VT_INT;

    if(str == SVT_FLOAT)
        return VT_FLOAT;

    if(str == SVT_REF)
        return VT_REF;

    throw std::invalid_argument("unknown value string");
}


// Literal Value or Variable

LVOV::LVOV()
{
    reset();
}


LVOV::LVOV(string str_val)
{
    reset();
    this->str_val = str_val;
    type = VT_STR;
}


LVOV::LVOV(string str_val, bool is_variable)
{
    reset();

    if(is_variable)
    {
        type = VT_REF;
        variable_name = str_val;
    }

    else
    {
        type = VT_STR;
        this->str_val = str_val;
    }
}


LVOV::LVOV(int int_val)
{
    reset();
    this->int_val = int_val;
    type = VT_INT;
}


LVOV::LVOV(float float_val)
{
    reset();
    this->float_val = float_val;
    type = VT_FLOAT;
}


void LVOV::reset()
{
    type = VT_ANY;
    variable_name = "";
    str_val = "";
    int_val = 0;
    float_val = 0.0;
}


string LVOV::to_string()
{
    if(type == VT_ANY)
        return "";

    if(type == VT_STR)
        return str_val;

    if(type == VT_INT)
        return std::to_string(int_val);

    if(type == VT_FLOAT)
        return std::to_string(float_val);

    if(type == VT_REF)
        return variable_name;

    throw std::invalid_argument("unknown ValueType");
}


bool LVOV::from_string(Glib::ustring input)
{
    if(type == VT_STR)
    {
        str_val = input;
        return true;
    }

    else if(type == VT_REF)
    {
        variable_name = input;
        return true;
    }

    else if(type == VT_FLOAT)
        return string_to_float(float_val, input);

    else if(type == VT_INT)
        return string_to_int(int_val, input);

    else
        throw std::invalid_argument("unknown ValueType");
}


LVOV::operator int() const
{
    return int_val;
}


LVOV::operator string() const
{
    return type == VT_STR ? str_val : variable_name;
}


LVOV::operator float() const
{
    return float_val;
}

