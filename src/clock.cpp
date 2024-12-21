#include "clock.h"

void tickRateClock::calculateDeltaTime() {
    const boost::chrono::steady_clock::time_point currentTime = now();
    const boost::chrono::duration<float> deltaTime = boost::chrono::duration_cast<boost::chrono::duration<float>>(
        currentTime - lastTime);
    this->lastTime = currentTime;
    this->deltaTime = deltaTime.count();
}

void tickRateClock::calculateFrameSwapDeltaTime() {
    const boost::chrono::steady_clock::time_point currentTime = now();
    const boost::chrono::duration<float> frameSwapDeltaTime = boost::chrono::duration_cast<boost::chrono::duration<float>>(
        currentTime - lastFrameSwapTime);
    this->frameSwapDeltaTime = frameSwapDeltaTime.count();
}

void tickRateClock::initialize() {
    this->lastTime = now();
}

void tickRateClock::resetFrameSwapTime() {
    lastFrameSwapTime = now();
}

float tickRateClock::floatTime() const {
    const boost::chrono::steady_clock::time_point currentTime = now();
    const boost::chrono::duration<float> elapsedTime = boost::chrono::duration_cast<boost::chrono::duration<float>>(currentTime.time_since_epoch());
    return elapsedTime.count();
}

boost::chrono::steady_clock::time_point tickRateClock::now() {
    return boost::chrono::steady_clock::now();
}
