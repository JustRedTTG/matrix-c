#ifndef RENDERER_H
#define RENDERER_H
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#endif
#include <options.h>
#include <GL/glx.h>


struct renderer {
    options *opts;
#ifdef __linux__
    Display* display;
    Window desktop;
    Window window;
    Window root;
    int screenNum;
    GLXFBConfig *fbcs;
    XVisualInfo *vi;
    XRenderPictFormat *pict;
    GLXFBConfig fbc;
    XSetWindowAttributes swa;
#else

#endif
};

#endif //RENDERER_H
