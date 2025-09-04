#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h> // for logging
#include <chrono>

class Timer {
    public:
        Timer(): last_time(std::chrono::high_resolution_clock::now()) {}
        float getElapsed() {
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_time).count() / 1000.f;
            last_time = now;
            printf("Elapsed: %f\n", elapsed);
            return elapsed;
        }
        
    private:
        std::chrono::_V2::high_resolution_clock::time_point last_time;
};

#endif // _TIMER_H_