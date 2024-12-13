#include <chrono>

#include "shader.h"
#include "renderer.h"
#include <iostream>
#include <options.h>
#include <thread>

#ifdef __linux__
#include "x11.h"
#endif

int main(const int argc, char *argv[]) {
    options *opts = parseOptions(argc, argv);
    auto *rnd = new renderer(opts);

    rnd->makeWindow();


    while (true) {
        rnd->getEvents();
        if (rnd->events->quit) {
            break;
        }

        glClearColor(0.4f, 1.0f, 0.2f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);

        rnd->swapBuffers();
    }

    rnd->destroy();
    return 0;
}
