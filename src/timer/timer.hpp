#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h> // for logging
#include <chrono>

class Timer {
    public:
        Timer(): last_time(std::chrono::high_resolution_clock::now()) {}
        float getElapsed();
        
    private:
        std::chrono::_V2::high_resolution_clock::time_point last_time;
};

#endif // _TIMER_H_