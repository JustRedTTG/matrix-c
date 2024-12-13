#include <chrono>

#include "shader.h"
#include "GL/gl.h"
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

    // simple red gl screen


    // Keep the window open for 3 seconds
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (duration >= 10) {
            break;
        }

        glClearColor(0.4f, 1.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();

        rnd->swapBuffers();
    }

    rnd->destroy();
    return 0;
}
