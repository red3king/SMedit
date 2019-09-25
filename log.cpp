#include <iostream>
#include <iomanip>
#include <mutex>

#include "log.h"


unsigned int logline = 0;
std::mutex log_mutex;

void log(std::string msg)
{
    log(LL_MSG, msg);
}


void log(LogLevel ll, std::string msg)
{
    log_mutex.lock();
    logline++;
    std::cout << std::setw(6) << std::setfill('0') << logline;
    std::cout << "\t" << msg << std::endl;    
    log_mutex.unlock();
}
