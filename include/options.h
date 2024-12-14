#ifndef OPTIONS_H
#define OPTIONS_H
#include <cstdint>

#include "help_message.h"

enum PostProcessingOptions {
    GHOSTING = 1 << 0
};

struct options {
    bool wallpaperMode = false;
    bool fullscreen = true;
    long width = 800;
    long height = 600;
    uint8_t postProcessingOptions = 0;
    uint8_t userAllowedPostProcessingOptions = 0xFF;
    char* app = new char[256];
};

options* parseOptions(int argc, char *argv[]);

#endif //OPTIONS_H
