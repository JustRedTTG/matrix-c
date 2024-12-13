#ifndef OPTIONS_H
#define OPTIONS_H
#include "help_message.h"

struct options {
    bool wallpaperMode = false;
    int width = 0;
    int height = 0;
};

options* parseOptions(int argc, char *argv[]);

#endif //OPTIONS_H
