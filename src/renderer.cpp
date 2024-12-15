#include "renderer.h"

#include <cstring>
#include <gl_errors.h>
#include <shader.h>
#include <vector>
#include "basic_texture_fragment_shader.h"
#include "basic_texture_vertex_shader.h"
#include "ghosting_fragment_shader.h"


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
            unsigned char mask[(XI_LASTEVENT + 7) / 8] = {0};
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

        GL_CHECK(glViewport(0, 0, opts->width, opts->height));

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
    // Create the framebuffers
    createFrameBufferTexture(fboC, fboCTexture, GL_RGBA);
    createFrameBufferTexture(fboM, fboMTexture, GL_RGBA);
    createFrameBufferTexture(fboP, fboPTexture, GL_RGBA);

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    // Create renderbuffer
    GL_CHECK(glGenRenderbuffers(1, &RBO));
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, opts->width, opts->height));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fboC));
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete" << std::endl;
        exit(1);
    }

    // Unbind the framebuffer
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void renderer::createFrameBufferTexture(GLuint &fbo, GLuint &fboTexture, const GLuint format) const {
    GL_CHECK(glCreateFramebuffers(1, &fbo));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    GL_CHECK(glGenTextures(1, &fboTexture));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, fboTexture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, opts->width, opts->height, 0, format, GL_UNSIGNED_BYTE, nullptr));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete" << std::endl;
        exit(1);
    }
}

void renderer::initializePP() {
    // Create VAO for full-screen quad
    GL_CHECK(glGenVertexArrays(1, &ppFullQuadArray));
    GL_CHECK(glBindVertexArray(ppFullQuadArray));

    GL_CHECK(glGenBuffers(1, &ppFullQuadBuffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, ppFullQuadBuffer));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(ppFullQuadBufferData), ppFullQuadBufferData, GL_STATIC_DRAW));

    GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat))));
    GL_CHECK(glEnableVertexAttribArray(1));

    // Create the final post-processing program
    ppFinalProgram = createProgram();
    loadShaderInternal(basicTextureVertexShader, sizeof(basicTextureVertexShader), GL_VERTEX_SHADER);
    loadShaderInternal(basicTextureFragmentShader, sizeof(basicTextureFragmentShader), GL_FRAGMENT_SHADER);
    linkProgram();

    // Create option specific post-processing programs
    if (opts->postProcessingOptions & GHOSTING) {
        ppGhostingProgram = createProgram();
        loadShaderInternal(basicTextureVertexShader, sizeof(basicTextureVertexShader), GL_VERTEX_SHADER);
        loadShaderInternal(ghostingFragmentShader, sizeof(ghostingFragmentShader), GL_FRAGMENT_SHADER);
        linkProgram();
    }

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
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
    opts->maskPostProcessingOptionsWithUserAllowed();
    initializePP();
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
    destroyApp();
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
        GL_CHECK(glDetachShader(program, vertexShader));
        GL_CHECK(glDeleteShader(vertexShader));
    }
    if (fragmentShader) {
        GL_CHECK(glDetachShader(program, fragmentShader));
        GL_CHECK(glDeleteShader(fragmentShader));
    }
    GL_CHECK(glDeleteFramebuffers(1, &fboC));
    GL_CHECK(glDeleteTextures(1, &fboCTexture));
    GL_CHECK(glDeleteFramebuffers(1, &fboM));
    GL_CHECK(glDeleteTextures(1, &fboMTexture));
    GL_CHECK(glDeleteFramebuffers(1, &fboP));
    GL_CHECK(glDeleteTextures(1, &fboPTexture));
    GL_CHECK(glDeleteRenderbuffers(1, &RBO));
    GL_CHECK(glDeleteVertexArrays(1, &ppFullQuadArray));
    GL_CHECK(glDeleteBuffers(1, &ppFullQuadBuffer));
    GL_CHECK(glDeleteProgram(ppFinalProgram));
    if (opts->postProcessingOptions & GHOSTING) {
        GL_CHECK(glDeleteProgram(ppGhostingProgram));
    }
    delete clock;
    delete events;
    delete opts;
}

void renderer::getEvents() const {
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
    checkGLError("glCreateProgram", __FILE__, __LINE__);
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
    GL_CHECK(glShaderSource(shader, 1, &source, nullptr));
    GL_CHECK(glCompileShader(shader));
    GLint success;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        GLint logLength;
        GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));
        std::vector<char> log(logLength);
        GL_CHECK(glGetShaderInfoLog(shader, logLength, &logLength, log.data()));
        std::cerr << "Shader compilation failed: " << log.data() << std::endl;
    }
    GL_CHECK(glAttachShader(program, shader));

    if (type == GL_VERTEX_SHADER)
        vertexShader = shader;
    else if (type == GL_FRAGMENT_SHADER)
        fragmentShader = shader;
}

void renderer::linkProgram() const {
    linkProgram(program);
}

void renderer::linkProgram(const GLuint program) {
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
    GL_CHECK(glUseProgram(program));
}

void renderer::useProgram() const {
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
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fboC));
    _clear();
}

void renderer::_clear() const {
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void renderer::clear() const {
    // if (opts->postProcessingOptions & GHOSTING)
    //     return;
    _clear();
}

void renderer::_swapPPBuffers() {
    GLuint temp = fboCTexture;
    fboCTexture = fboMTexture;
    fboMTexture = temp;
    temp = fboC;
    fboC = fboM;
    fboM = temp;
}

void renderer::frameEnd() {
    // Handle post-processing
    if (opts->postProcessingOptions & GHOSTING) {
        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fboM));
        _clear();
        useProgram(ppGhostingProgram);

        GL_CHECK(glUniform1i(glGetUniformLocation(ppGhostingProgram, "u_textureC"), 0));
        GL_CHECK(glUniform1i(glGetUniformLocation(ppGhostingProgram, "u_textureP"), 1));
        GL_CHECK(glBindVertexArray(ppFullQuadArray));

        // Bind the framebuffer textures
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fboCTexture));

        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fboPTexture));

        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

        _swapPPBuffers();
    }

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    _clear();
    useProgram(ppFinalProgram);
    GL_CHECK(glBindVertexArray(ppFullQuadArray));

    GL_CHECK(glUniform1i(glGetUniformLocation(ppFinalProgram, "u_texture"), 0));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, fboCTexture));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

    // Swap the framebuffers
    GLuint temp = fboPTexture;
    fboPTexture = fboCTexture;
    fboCTexture = temp;
    temp = fboP;
    fboP = fboC;
    fboC = temp;
}
