#ifndef EVENTS_H
#define EVENTS_H

struct groupedEvents;
#ifdef __linux__
#include <iostream>
#include <renderer.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

void handleX11Events(const renderer *rnd);
void handleGLFWEvents(const renderer *rnd);
#endif

struct groupedEvents {
    bool quit;
    long mouseX, mouseY;
    bool mouseLeft, mouseRight, mouseMiddle;
    std::chrono::time_point<std::chrono::system_clock> lastMouseMotion{};
};

#endif //EVENTS_H
