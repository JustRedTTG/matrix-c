#include "apps.h"

#include <cstring>
#include <iostream>
#include <apps/triangle.h>

App::App(renderer *rnd) {
    this->rnd = rnd;
}

App *initializeApp(renderer *rnd, const char *name) {
    if (strcmp(name, "triangle") == 0) {
        const auto app = new TriangleApp(rnd);
        app->setup();
        return app;
    } else {
        std::cerr << "Unknown app: " << name << std::endl;
        exit(1);
    }
}