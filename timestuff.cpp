#include "timestuff.h"



int timedelta_ms(millitime now, millitime then)
{
    return duration_cast<milliseconds>(now - then).count();
}

