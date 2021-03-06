#include "lvov.h"


// Literal Value or Variable

LVOV::LVOV()
{
    reset();
}



LVOV::LVOV(ValueType value_type)
{
    reset();
    type = value_type;
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


LVOV::LVOV(json jdata)
{
    type = (ValueType) jdata["type"];
    string val = jdata["value"];
    from_string(val);
}


json LVOV::to_json()
{
    return {
        { "type", (int)(type) },
        { "value", to_string() }    
    };
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
        return str_val;

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


string LVOV::describe()
{
    string prefix;

    if(type == VT_ANY)
        prefix = str_val;

    else if(type == VT_STR)
        prefix = "\"" + str_val + "\"";

    else if(type == VT_INT)
        prefix = std::to_string(int_val);

    else if(type == VT_FLOAT)
        prefix = std::to_string(float_val);

    else if(type == VT_REF)
        prefix = "`" + variable_name + "`";
    else
        throw std::invalid_argument("unknown ValueType");

    return prefix + " (" + value_type_to_string(type) + ")";
}


bool LVOV::from_string(Glib::ustring input)
{
    if(type == VT_STR || type == VT_ANY)
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

