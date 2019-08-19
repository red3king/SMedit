#include <stdexcept>

#include "const.h"


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


