#include "clock.h"

void tickRateClock::calculateDeltaTime() {
    const boost::chrono::steady_clock::time_point currentTime = now();
    const boost::chrono::duration<float> deltaTime = boost::chrono::duration_cast<boost::chrono::duration<float>>(
        currentTime - lastTime);
    this->lastTime = currentTime;
    this->deltaTime = deltaTime.count();
}

void tickRateClock::initialize() {
    this->lastTime = now();
}

boost::chrono::steady_clock::time_point tickRateClock::now() {
    return boost::chrono::steady_clock::now();
}
