#ifndef SHADER_H
#define SHADER_H
// ReSharper disable once CppUnusedIncludeDirective
#include <string>
#include <sstream>
#include <array>
#include <GL/glew.h>

std::array<std::stringstream, 2> parseShader(const std::string *source);

std::array<std::stringstream, 2> parseShader(const unsigned char *source, int length);

enum ShaderType {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
};

#endif //SHADER_H
