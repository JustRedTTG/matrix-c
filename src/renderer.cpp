#include "renderer.h"

#include <cstring>

#include "x11.h"

auto glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(
    glXGetProcAddressARB(
        reinterpret_cast<const GLubyte *>("glXCreateContextAttribsARB")));

renderer::renderer(options *opts) {
    this->opts = opts;
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

        this->ctx = glXCreateContextAttribsARB(this->display, this->fbc, NULL, True, gl3attr);

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




}

void renderer::swapBuffers() {
#ifdef __linux
    if (this->x11) {
        x11_SwapBuffers(this);
        return;
    }
#endif
    glfwSwapBuffers(this->glfwWindow);
}

void renderer::destroy() {
    if (this->x11) {
        XCloseDisplay(this->display);
    } else {
        glfwDestroyWindow(this->glfwWindow);
        glfwTerminate();
    }
}