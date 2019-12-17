#pragma once

#include <chrono>

using namespace std::chrono;
typedef high_resolution_clock::time_point millitime;

int timedelta_ms(millitime now, millitime then);

