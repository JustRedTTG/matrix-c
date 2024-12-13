#ifndef RENDERER_H
#define RENDERER_H
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#endif
#include <options.h>
#include <GLFW/glfw3.h>
#include <GL/glx.h>

#define TITLE "Matrix rain"

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);


struct renderer {
    options *opts;
#ifdef __linux__
    Display *display = nullptr;
    Window desktop{};
    Window window{};
    Window root{};
    int screenNum = -1;
    GLXFBConfig *fbcs = nullptr;
    XVisualInfo *vi = nullptr;
    XRenderPictFormat *pict = nullptr;
    GLXFBConfig fbc{};
    XSetWindowAttributes swa{};
    bool x11 = false;
#else

#endif
    GLFWwindow *glfwWindow = nullptr;
    GLXContext ctx{};

    void makeWindow();

    explicit renderer(options *opts);

    void swapBuffers();
    void destroy();
};

#endif //RENDERER_H
