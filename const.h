#pragma once

#include <string>

using std::string;

#define SVT_ANY "any"
#define SVT_STR "string"
#define SVT_INT "int"
#define SVT_FLOAT "float"
#define SVT_REF "reference"


enum ValueType { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };
static const ValueType AllValueTypes[] = { VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF };

#define NUM_VALUE_TYPES (sizeof(AllValueTypes)/sizeof(*AllValueTypes))

string value_type_to_string(ValueType vt);
ValueType string_to_value_type(string str);


