#ifndef CLOCK_H
#define CLOCK_H
#include <chrono>

struct tickRateClock {
    std::chrono::time_point<std::chrono::system_clock> lastTime{};
    float deltaTime{};

    void calculateDeltaTime();
    void initialize();
};


#endif //CLOCK_H
