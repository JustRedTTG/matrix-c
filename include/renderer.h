#ifndef RENDERER_H
#define RENDERER_H
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#endif
#include <clock.h>
#include <options.h>
#include <events.h>
#include <GL/glew.h>
#include <GL/gl.h>
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
    tickRateClock *clock;
    GLFWwindow *glfwWindow = nullptr;
    GLXContext ctx{};
    GLuint program{};
    GLuint vertexShader{};
    GLuint fragmentShader{};

    void makeWindow();

    void getEvents();

    void createProgram();

    void loadShader(const unsigned char *source, int length, GLuint type);
    void loadShader(const char *source, GLuint type);

    void linkProgram() const;

    void loadShader(const unsigned char *source, int length);

    void useProgram() const;

    groupedEvents *events = nullptr;

    explicit renderer(options *opts);

    void swapBuffers();
    void destroy() const;
};

#endif //RENDERER_H
