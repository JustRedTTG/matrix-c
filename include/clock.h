#ifndef CLOCK_H
#define CLOCK_H
#include <boost/chrono.hpp>

struct tickRateClock {
    boost::chrono::steady_clock::time_point lastTime{};
    float deltaTime{};

    void calculateDeltaTime();
    void initialize();

    static boost::chrono::steady_clock::time_point now();
};

#endif //CLOCK_H
