#ifndef CLOCK_H
#define CLOCK_H
#include <chrono>

struct tickRateClock {
    std::chrono::time_point<std::chrono::system_clock> lastTime{};
    float deltaTime{};

    void calculateDeltaTime();
    void initialize();

    std::chrono::time_point<std::chrono::system_clock> now();
};

#endif //CLOCK_H
