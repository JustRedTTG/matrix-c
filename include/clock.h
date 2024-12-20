#ifndef CLOCK_H
#define CLOCK_H
#include <boost/chrono.hpp>

struct tickRateClock {
    boost::chrono::steady_clock::time_point lastTime{};
    boost::chrono::steady_clock::time_point lastFrameSwapTime{};
    float deltaTime{};
    float frameSwapDeltaTime{};

    void calculateDeltaTime();
    void initialize();
    void resetFrameSwapTime();

    static boost::chrono::steady_clock::time_point now();
};

#endif //CLOCK_H
