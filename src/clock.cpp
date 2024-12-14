//
// Created by red on 12/13/24.
//

#include "clock.h"

#include <chrono>

void tickRateClock::calculateDeltaTime() {
    const std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();  // Get the current time
    const std::chrono::duration<float> deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime);  // Calculate delta time in seconds
    this->lastTime = currentTime;  // Update last time
    this->deltaTime = deltaTime.count();
}

void tickRateClock::initialize() {
    this->lastTime = std::chrono::high_resolution_clock::now();
}