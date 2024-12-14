#ifndef OPTIONS_H
#define OPTIONS_H
#include "help_message.h"

struct options {
    bool wallpaperMode = false;
    bool fullscreen = true;
    int width = 800;
    int height = 600;
    const char* app = "triangle";
};

options* parseOptions(int argc, char *argv[]);

#endif //OPTIONS_H
