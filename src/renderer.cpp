#include "renderer.h"

#include <cstring>
#include <shader.h>
#include <vector>


#ifdef __linux__
#include "x11.h"
#include <csignal>
#include <signal.h>
#endif

auto glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(
    glXGetProcAddressARB(
        reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));

renderer *renderer::instance = nullptr;

renderer::renderer(options *opts) {
    this->opts = opts;
    clock = new tickRateClock();
    events = new groupedEvents();
    instance = this;
}

void initializeGlew() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Couldn't initialize GLEW" << std::endl;
        exit(1);
    }
}
#ifdef __linux__
void renderer::handleSignal(const int signal) {
    if (signal == SIGINT || signal == SIGTERM || signal == SIGSTOP) {
        instance->events->quit = true;
    }
}

void renderer::setupSignalHandling() {
    const auto handler = handleSignal;
    std::signal(SIGINT, handler);
    std::signal(SIGTERM, handler);
    std::signal(SIGSTOP, handler);
}
#endif


void createFrameBuffer(GLuint &fbo) {
    glCreateFramebuffers(1, &fbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete" << std::endl;
        exit(1);
    }
}

void renderer::makeContext() {
    if (opts->wallpaperMode) {
#ifdef __linux__
        display = XOpenDisplay(nullptr);
        setupWindowForWallpaperMode(this);
        int gl3attr[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        ctx = glXCreateContextAttribsARB(display, fbc, nullptr, True, gl3attr);

        if (!ctx) {
            printf("Couldn't create an OpenGL context\n");
            exit(1);
        }

        XTextProperty windowName;
        windowName.value = reinterpret_cast<unsigned char *>(const_cast<char *>(TITLE));
        windowName.encoding = XA_STRING;
        windowName.format = 8;
        windowName.nitems = strlen(reinterpret_cast<char *>(windowName.value));

        XSetWMName(display, window, &windowName);

        XMapWindow(display, window);

        int event, error;
        if (!XQueryExtension(display, "XInputExtension", &xinputOptCode, &event, &error)) {
            std::cerr << "X Input extension not available" << std::endl;
            XSelectInput(
                display, window,
                ExposureMask |
                KeyPressMask | KeyReleaseMask |
                StructureNotifyMask |
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask
            );
        } else {
            x11MouseEvents = true;
            XIEventMask evmask;
            unsigned char mask[(XI_LASTEVENT + 7)/8] = { 0 };
            evmask.deviceid = XIAllDevices;
            evmask.mask_len = sizeof(mask);
            evmask.mask = mask;
            XISetMask(mask, XI_RawMotion);
            XISetMask(mask, XI_RawKeyPress);
            XISetMask(mask, XI_RawKeyRelease);
            XISetMask(mask, XI_RawButtonPress);
            XISetMask(mask, XI_RawButtonRelease);

            XISelectEvents(this->display, DefaultRootWindow(this->display), &evmask, 1);
        }

        glXMakeCurrent(display, window, ctx);

        glViewport(0, 0, opts->width, opts->height);

        x11 = true;

#else
        std::cerr << "Wallpaper mode is only supported on Linux" << std::endl;
        exit(1);
#endif
        return;
    }
    // Create a normal GLFW context

    if (!glfwInit()) {
        std::cerr << "Couldn't initialize GLFW" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    if (opts->fullscreen) {
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
        glfwWindow = glfwCreateWindow(mode->width, mode->height, TITLE, primaryMonitor, nullptr);
        opts->width = mode->width;
        opts->height = mode->height;
    } else {
        glfwWindow = glfwCreateWindow(opts->width, opts->height, TITLE, nullptr, nullptr);
    }

    if (!glfwWindow) {
        const char *description;
        int code = glfwGetError(&description);
        std::cerr << "Couldn't create a GLFW window: " << description << " (Error code: " << code << ")" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(glfwWindow);
}

void renderer::makeFrameBuffers() {
    createFrameBuffer(fboC);
    createFrameBuffer(fboM);
    createFrameBuffer(fboP);
}

void renderer::initialize() {
#ifdef __linux__
    setupSignalHandling();
#endif
    makeContext();
    initializeGlew();
    makeFrameBuffers();
    clock->initialize();
    loadApp();
}

void renderer::swapBuffers() {
    glFlush();
#ifdef __linux__
    if (x11) {
        x11_SwapBuffers(this);
        return;
    }
#endif
    glfwSwapBuffers(glfwWindow);
}

void renderer::destroy() const {
#ifdef __linux__
    if (x11) {
        XCloseDisplay(display);
    }
#else
    if () {}
#endif
    else {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    if (vertexShader) {
        glDetachShader(program, vertexShader);
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDetachShader(program, fragmentShader);
        glDeleteShader(fragmentShader);
    }
    glDeleteProgram(program);
    glDeleteFramebuffers(1, &fboC);
    glDeleteFramebuffers(1, &fboM);
    glDeleteFramebuffers(1, &fboP);
}

void renderer::getEvents() {
    clock->calculateDeltaTime();
#ifdef __linux__
    if (x11) {
        handleX11Events(this);
        return;
    }
#endif
    handleGLFWEvents(this);
}

GLuint renderer::createProgram() {
    program = glCreateProgram();
    return program;
}

void renderer::loadShaderInternal(const unsigned char *source, int length, const GLuint type) {
    loadShader(source, length, type, program);
}

void renderer::loadShader(const unsigned char *source, int length, GLuint type, GLuint program) {
    const std::string src(reinterpret_cast<const char *>(source), length);
    return loadShader(src.c_str(), type, program);
}

void renderer::loadShader(const char *source, const GLuint type) {
    loadShader(source, type, program);
}

void renderer::loadShader(const char *source, GLuint type, GLuint program) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log.data());
        std::cerr << "Shader compilation failed: " << log.data() << std::endl;
    }
    glAttachShader(program, shader);

    if (type == GL_VERTEX_SHADER)
        vertexShader = shader;
    else if (type == GL_FRAGMENT_SHADER)
        fragmentShader = shader;
}

void renderer::linkProgram() const {
    linkProgram(program);
}

void renderer::linkProgram(const GLuint program) const {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glLinkProgram(program);

    // Check the program
    glGetProgramiv(program, GL_LINK_STATUS, &Result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
}

void renderer::loadShader(const unsigned char *source, const int length) {
    loadShader(source, length, program);
}

void renderer::loadShader(const unsigned char *source, int length, GLuint program) {
    const std::array<std::stringstream, 2> sources = parseShader(source, length);
    const std::string vertexSource = sources[0].str();
    const std::string fragmentSource = sources[1].str();

    loadShader(vertexSource.c_str(), GL_VERTEX_SHADER);
    loadShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

    linkProgram(program);
}

void renderer::useProgram(const GLuint program) {
    glUseProgram(program);
}

void renderer::useProgram() {
    useProgram(program);
}

void renderer::loadApp() {
    app = initializeApp(this, opts->app);
}

void renderer::loopApp() const {
    app->loop();
}

void renderer::destroyApp() const {
    app->destroy();
    delete app;
}

void renderer::frameBegin() const {
    // glBindFramebuffer(GL_FRAMEBUFFER, fboC);
}

void renderer::frameEnd() const {
    // glBindFramebuffer(GL_FRAMEBUFFER, fboM);
}
