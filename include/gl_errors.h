#ifndef GL_ERRORS_H
#define GL_ERRORS_H
#define GL_CHECK(call) \
    do { \
        call; \
        checkGLError(#call, __FILE__, __LINE__); \
    } while (0)

void checkGLError(const char *call, const char *file, int line);
#endif //GL_ERRORS_H
