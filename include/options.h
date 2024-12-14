#ifndef OPTIONS_H
#define OPTIONS_H
#include "help_message.h"

struct options {
    bool wallpaperMode = false;
    bool fullscreen = true;
    long width = 800;
    long height = 600;
    char* app = new char[256];
};

options* parseOptions(int argc, char *argv[]);

#endif //OPTIONS_H
