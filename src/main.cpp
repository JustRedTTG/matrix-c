#include "shader.h"
#include "GL/gl.h"
#include <iostream>
#include <options.h>

#ifdef __linux__
#include "x11.h"
#endif

#define TITLE "Matrix"

int main(const int argc, char *argv[]) {
    options *opts = parseOptions(argc, argv);
    renderer rnd = {opts};



    return 0;
}
