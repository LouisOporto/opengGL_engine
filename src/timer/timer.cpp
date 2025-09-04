#include "timer.hpp"

float Timer::getElapsed() {
    auto now = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_time).count() / 1000.f;
    last_time = now;
    printf("Elapsed: %f\n", elapsed);
    return elapsed;
}

