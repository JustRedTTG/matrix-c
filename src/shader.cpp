#include "shader.h"

void loadShader(const char **src, int type, int program) {
    int shader = glCreateShader(type);
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);
    glAttachShader(program, shader);
}
