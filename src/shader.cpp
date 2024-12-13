#include "shader.h"

std::array<std::stringstream, 2> parseShader(const std::string *source) {
    std::istringstream stream(*source);
    std::string line;
    std::array<std::stringstream, 2> ss;
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else
            ss[static_cast<int>(type)] << line << '\n';
    }
    return ss;
}

std::array<std::stringstream, 2> parseShader(const unsigned char *source, const int length) {
    const std::string str(reinterpret_cast<const char*>(source), length);
    return parseShader(&str);
}