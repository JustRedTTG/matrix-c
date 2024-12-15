#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <apps.h>
#include <triangle_shader.h>
#include <GL/glew.h>

class TriangleApp final : public App {
public:
    explicit TriangleApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
protected:
    float t = 0;
    int m = 1;
    GLuint vertexArray{};
    GLuint vertexBuffer{};
    GLuint ui_Time{};
    GLuint program{};
};

#endif //TRIANGLE_H
