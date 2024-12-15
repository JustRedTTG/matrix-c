#include "clock.h"

void tickRateClock::calculateDeltaTime() {
    const std::chrono::time_point<std::chrono::system_clock> currentTime = now();
    const std::chrono::duration<float> deltaTime = std::chrono::duration_cast<std::chrono::duration<float> >(
        currentTime - lastTime);
    this->lastTime = currentTime; // Update last time
    this->deltaTime = deltaTime.count();
}

void tickRateClock::initialize() {
    this->lastTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        std::chrono::high_resolution_clock::now());
}

std::chrono::time_point<std::chrono::system_clock> tickRateClock::now() {
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(std::chrono::high_resolution_clock::now());
}
