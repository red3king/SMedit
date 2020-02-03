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


enum BorderType { BT_NONE, BT_TOP, BT_RIGHT, BT_BOTTOM, BT_LEFT };



#define SERV_RESP_SUCCESS 1
#define SERV_RESP_FAILURE 0



// transition model

enum TransitionType { EVENT, TIMEOUT, CATCHALL };
enum CornerPosition { CP_NONE=-1, CP_TL=0, CP_TR=1, CP_BL=2, CP_BR=3 };

