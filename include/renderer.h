#ifndef RENDERER_H
#define RENDERER_H
#include <apps.h>
#include <clock.h>
#include <options.h>
#include <events.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <GL/glx.h>
#endif



#define TITLE "Matrix rain"

#ifdef __linux__
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
#endif

static constexpr GLfloat ppFullQuadBufferData[] = {
    // Coordinates    // Texture coordinates
     1.0f, -1.0f,     1.0f, 0.0f,
    -1.0f, -1.0f,     0.0f, 0.0f,
    -1.0f,  1.0f,     0.0f, 1.0f,

     1.0f,  1.0f,     1.0f, 1.0f,
     1.0f, -1.0f,     1.0f, 0.0f,
    -1.0f,  1.0f,     0.0f, 1.0f
};

struct renderer {
    options *opts;
#ifdef __linux__
    Display *display = nullptr;
    Window desktop{};
    Window window{};
    Window root{};
    GLXContext ctx{};
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

    GLuint program{};
    GLuint vertexShader{};
    GLuint fragmentShader{};

    GLuint ppGhostingProgram{};
    GLuint ppBlurProgram{};

    GLuint ppFinalProgram{};

    GLuint ppFullQuadArray{};
    GLuint ppFullQuadBuffer{};

    GLuint fboC{};
    GLuint fboCTexture{};
    GLuint fboM{};
    GLuint fboMTexture{};
    GLuint fboP{};
    GLuint fboPTexture{};

    GLuint fboCOutput{};
    GLuint fboCTextureOutput{};
    GLuint fboMOutput{};
    GLuint fboMTextureOutput{};
    GLuint fboPOutput{};
    GLuint fboPTextureOutput{};
    GLuint RBO{};

    void makeWindow();

    void getEvents() const;

    GLuint createProgram();

    void loadShaderInternal(const unsigned char *source, int length, GLuint type);
    void loadShader(const unsigned char *source, int length, GLuint type, GLuint program);

    void loadShader(const char *source, GLuint type);
    void loadShader(const char *source, GLuint type, GLuint program);

    void linkProgram() const;
    static void linkProgram(GLuint program);

    void loadShader(const unsigned char *source, int length);
    void loadShader(const unsigned char *source, int length, GLuint program);

    static void useProgram(GLuint program);
    void useProgram() const;

    void loadApp();
    void loopApp() const;
    void destroyApp() const;

    void frameBegin() const;

    void _clear() const;

    void clear() const;

    void _swapPPBuffers();

    void _resolveMultisampledFramebuffer(GLuint srcFbo, GLuint dstFbo) const;

    void _sampleFrameBuffersForPostProcessing() const;

    void frameEnd();

    groupedEvents *events = nullptr;

    static renderer *instance;

    explicit renderer(options *opts);

    void makeContext();

    void makeFrameBuffers();
    void createFrameBufferTexture(GLuint &fbo, GLuint &fboTexture, GLuint format, bool multiSampled) const;

    void initializePP();

    void initialize();

    void swapBuffers();
    void destroy() const;
};

#endif //RENDERER_H
