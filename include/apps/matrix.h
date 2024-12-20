#ifndef MATRIX_H
#define MATRIX_H
#include <apps.h>
#include <fonts.h>
#include <triangle_shader.h>
#include <freetype/freetype.h>
#include "matrix_shader.h"

#include "glad.h"

#define MATRIX_CHARACTERS "ﾊﾐﾋｰｳｼﾅﾓﾆｻﾜﾂｵﾘｱﾎﾃﾏｹﾒｴｶｷﾑﾕﾗｾﾈｽﾀﾇﾍ012345789Z:・.\"=*+-<>¦｜"
#define MATRIX_RAIN_LIMIT 2000

struct Vertex {
    float x, y;
    float colorOffset;
    int spark;
};

class MatrixApp final : public App {
public:
    explicit MatrixApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
private:
    ShaderProgram *program{};
    FT_Face font{};
    GLuint glyphMatrixBuffer{}, glyphMatrixTexture{};
    GLuint ui_BaseColor{};
    GLuint vertexArray{}, vertexBuffer{};
    std::vector<Vertex> vertices;
};


#endif //MATRIX_H
