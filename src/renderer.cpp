#include "renderer.h"

#include <cstring>
#include <shader.h>
#include <vector>

#ifdef __linux__
#include "x11.h"
#endif

auto glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(
    glXGetProcAddressARB(
        reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));

renderer::renderer(options *opts) {
    this->opts = opts;
    this->clock = new tickRateClock();
}

void initializeGlew() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Couldn't initialize GLEW" << std::endl;
        exit(1);
    }
}

void renderer::makeWindow() {
    if (this->opts->wallpaperMode) {
#ifdef __linux__
        this->display = XOpenDisplay(nullptr);
        setupWindowForWallpaperMode(this);
        int gl3attr[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        this->ctx = glXCreateContextAttribsARB(this->display, this->fbc, nullptr, True, gl3attr);

        if (!this->ctx) {
            printf("Couldn't create an OpenGL context\n");
            exit(1);
        }

        XTextProperty windowName;
        windowName.value = reinterpret_cast<unsigned char *>(const_cast<char *>(TITLE));
        windowName.encoding = XA_STRING;
        windowName.format = 8;
        windowName.nitems = strlen(reinterpret_cast<char *>(windowName.value));

        XSetWMName(this->display, this->window, &windowName);

        XMapWindow(this->display, this->window);
        glXMakeCurrent(this->display, this->window, this->ctx);

        glViewport(0, 0, this->opts->width, this->opts->height);

        this->x11 = true;
        initializeGlew();
        this->clock->initialize();

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
    if (this->opts->fullscreen) {
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
        this->glfwWindow = glfwCreateWindow(mode->width, mode->height, TITLE, primaryMonitor, nullptr);
    } else {
        this->glfwWindow = glfwCreateWindow(this->opts->width, this->opts->height, TITLE, nullptr, nullptr);
    }

    if (!this->glfwWindow) {
        const char *description;
        int code = glfwGetError(&description);
        std::cerr << "Couldn't create a GLFW window: " << description << " (Error code: " << code << ")" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(this->glfwWindow);

    initializeGlew();
    this->clock->initialize();
}

void renderer::swapBuffers() {
    glFlush();
#ifdef __linux__
    if (this->x11) {
        x11_SwapBuffers(this);
        return;
    }
#endif
    glfwSwapBuffers(this->glfwWindow);
}

void renderer::destroy() const {
#ifdef __linux__
    if (this->x11) {
        XCloseDisplay(this->display);
        return;
    }
#endif

    if (this->vertexShader) {
        glDetachShader(this->program, this->vertexShader);
        glDeleteShader(this->vertexShader);
    }
    if (this->fragmentShader) {
        glDetachShader(this->program, this->fragmentShader);
        glDeleteShader(this->fragmentShader);
    }
    glDeleteProgram(this->program);
    glfwDestroyWindow(this->glfwWindow);
    glfwTerminate();
}

void renderer::getEvents() {
    this->clock->calculateDeltaTime();
    this->events = new groupedEvents();
#ifdef __linux__
    if (this->x11) {
        XEvent event;
        if (XPending(this->display) > 0) {
            XNextEvent(this->display, &event);
            this->events->quit = event.type != DestroyNotify;
        }
        return;
    }
#endif
    // GLFW events
    glfwPollEvents();
    if (glfwWindowShouldClose(this->glfwWindow)) {
        this->events->quit = true;
    }
}

void renderer::createProgram() {
    this->program = glCreateProgram();
}

void renderer::loadShader(const unsigned char *source, int length, const GLuint type) {
    const std::string src(reinterpret_cast<const char *>(source), length);
    return loadShader(src.c_str(), type);
}

void renderer::loadShader(const char *source, const GLuint type) {
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
    glAttachShader(this->program, shader);

    if (type == GL_VERTEX_SHADER) {
        this->vertexShader = shader;
    } else if (type == GL_FRAGMENT_SHADER) {
        this->fragmentShader = shader;
    }
}

void renderer::linkProgram() const {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glLinkProgram(this->program);

    // Check the program
    glGetProgramiv(this->program, GL_LINK_STATUS, &Result);
    glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(this->program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
}

void renderer::loadShader(const unsigned char *source, const int length) {
    const std::array<std::stringstream, 2> sources = parseShader(source, length);
    const std::string vertexSource = sources[0].str();
    const std::string fragmentSource = sources[1].str();

    loadShader(vertexSource.c_str(), GL_VERTEX_SHADER);
    loadShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

    this->linkProgram();
}

void renderer::useProgram() const {
    glUseProgram(this->program);
}
