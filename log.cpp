#include <iostream>
#include <iomanip>

#include "log.h"


unsigned int logline = 0;

void log(std::string msg)
{
    log(LL_MSG, msg);
}


void log(LogLevel ll, std::string msg)
{
    logline++;
    std::cout << std::setw(6) << std::setfill('0') << logline;
    std::cout << "\t" << msg << std::endl;    
}
