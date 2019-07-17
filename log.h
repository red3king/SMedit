#pragma once

#include <string>


enum LogLevel { LL_ERR, LL_WARN, LL_MSG, LL_DEBUG };

void log(std::string msg);   // defaults to LL_MSG
void log(LogLevel ll, std::string msg);
