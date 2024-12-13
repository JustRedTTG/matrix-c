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
        if (arg == "-h") {
            showHelp();
            exit(0);
        } else if (arg == "-w") {
            opts->wallpaperMode = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            showHelp();
            exit(1);
        }
    }
    return opts;
}
