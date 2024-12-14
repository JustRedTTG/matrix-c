#ifndef RENDERER_H
#define RENDERER_H
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#endif
#include <apps.h>
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
    bool x11MouseEvents = false;
    int xinputOptCode;
    static void handleSignal(int signal);
    void setupSignalHandling();
#else

#endif
    int antialiasSamples = 4;
    App *app;
    tickRateClock *clock;
    GLFWwindow *glfwWindow = nullptr;
    GLXContext ctx{};
    GLuint program{};
    GLuint vertexShader{};
    GLuint fragmentShader{};
    GLuint fboC{};
    GLuint fboM{};
    GLuint fboP{};

    void makeWindow();

    void getEvents();

    GLuint createProgram();

    void loadShaderInternal(const unsigned char *source, int length, GLuint type);
    void loadShader(const unsigned char *source, int length, GLuint type, GLuint program);

    void loadShader(const char *source, GLuint type);
    void loadShader(const char *source, GLuint type, GLuint program);

    void linkProgram() const;
    void linkProgram(GLuint program) const;

    void loadShader(const unsigned char *source, int length);
    void loadShader(const unsigned char *source, int length, GLuint program);

    void useProgram(GLuint program);
    void useProgram();

    void loadApp();
    void loopApp() const;
    void destroyApp() const;

    void frameBegin() const;

    void frameEnd() const;

    groupedEvents *events = nullptr;

    static renderer *instance;

    explicit renderer(options *opts);

    void makeContext();

    void makeFrameBuffers();

    void initialize();

    void swapBuffers();
    void destroy() const;
};

#endif //RENDERER_H
