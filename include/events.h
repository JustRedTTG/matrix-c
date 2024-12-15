#ifndef EVENTS_H
#define EVENTS_H

struct groupedEvents;
#include <renderer.h>
#include <iostream>

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

void handleX11Events(const renderer *rnd);
#endif

void handleGLFWEvents(const renderer *rnd);

struct groupedEvents {
    bool quit;
    long mouseX, mouseY;
    bool mouseLeft, mouseRight, mouseMiddle;
    std::chrono::time_point<std::chrono::system_clock> lastMouseMotion{};
};

#endif //EVENTS_H
