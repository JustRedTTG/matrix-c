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

    rnd->initialize();
    rnd->loadApp();

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (true) {
        rnd->getEvents();
        if (rnd->events->quit) {
            break;
        }
        rnd->frameBegin();
        rnd->loopApp();
        rnd->swapBuffers();
    }

    rnd->destroyApp();
    rnd->destroy();
    return 0;
}
