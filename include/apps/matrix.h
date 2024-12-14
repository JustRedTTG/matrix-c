#ifndef MATRIX_H
#define MATRIX_H
#include <apps.h>
#include <triangle_shader.h>
#include <GL/glew.h>

class MatrixApp final : public App {
public:
    explicit MatrixApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
};
#endif //MATRIX_H
