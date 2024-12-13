#include "options.h"

#include <iostream>
#include <string>
#include <cstdio>

void showHelp() {
    std::cout << helpMessage << std::endl;
}

options* parseOptions(int argc, char *argv[]) {
    auto *opts = new options();
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            showHelp();
            exit(0);
        } else if (arg == "-w") {
            opts->wallpaperMode = true;
        } else if (arg == "-m") {
            opts->fullscreen = false;
        } else if (arg.find("--width=") == 0) {
            sscanf(argv[i], "--width=%d", &opts->width);
            opts->fullscreen = false;
        } else if (arg.find("--height=") == 0) {
            sscanf(argv[i], "--height=%d", &opts->height);
            opts->fullscreen = false;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            showHelp();
            exit(1);
        }
    }
    return opts;
}
