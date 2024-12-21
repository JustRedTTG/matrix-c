#ifndef CLOCK_H
#define CLOCK_H
#include <boost/chrono.hpp>

struct tickRateClock {
    boost::chrono::steady_clock::time_point lastTime{};
    boost::chrono::steady_clock::time_point lastFrameSwapTime{};
    float deltaTime{};
    float frameSwapDeltaTime{};

    void calculateDeltaTime();

    void calculateFrameSwapDeltaTime();

    void initialize();
    void resetFrameSwapTime();

    float floatTime() const;

    static boost::chrono::steady_clock::time_point now();
};

#endif //CLOCK_H
