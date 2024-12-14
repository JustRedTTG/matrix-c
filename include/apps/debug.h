#ifndef DEBUG_H
#define DEBUG_H
#include <apps.h>
#include <GL/glew.h>

class DebugApp final : public App {
public:
    explicit DebugApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
private:
    GLfloat vertices[8];
    GLuint vertexBuffer{};
    GLuint indexBuffer{};
    GLuint ui_MousePosition{};
    GLuint program{};
};
#endif //DEBUG_H
