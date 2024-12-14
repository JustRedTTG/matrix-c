#include "options.h"

#include <chrono>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <thread>
#define DEFAULT_APP "triangle"

void showHelp() {
    std::string helpText(reinterpret_cast<const char*>(helpMessage), sizeof(helpMessage));
    std::cout << helpText << std::endl;
}

options* parseOptions(int argc, char *argv[]) {
    auto *opts = new options();
    bool hasSetApp = false;
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
            opts->width = strtol(argv[i] + 8, nullptr, 10);
            opts->fullscreen = false;
        } else if (arg.find("--height=") == 0) {
            opts->height = strtol(argv[i] + 9, nullptr, 10);
            opts->fullscreen = false;
        } else if (arg.find("--app=") == 0) {
            sscanf(argv[i], "--app=%255s", opts->app);
            hasSetApp = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            showHelp();
            exit(1);
        }
    }
    if (!hasSetApp) {
        opts->app = new char[sizeof(DEFAULT_APP)];
        strcpy(opts->app, DEFAULT_APP);
    }

    return opts;
}
