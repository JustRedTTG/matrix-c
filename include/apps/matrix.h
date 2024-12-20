#ifndef MATRIX_H
#define MATRIX_H
#include <apps.h>
#include <fonts.h>
#include <triangle_shader.h>
#include <freetype/freetype.h>
#include "matrix_shader.h"

#include "glad.h"

// #define MATRIX_CHARACTERS "ﾊﾐﾋｰｳｼﾅﾓﾆｻﾜﾂｵﾘｱﾎﾃﾏｹﾒｴｶｷﾑﾕﾗｾﾈｽﾀﾇﾍ012345789Z:・.\"=*+-<>¦｜"
#define MATRIX_CHARACTERS "ｵ"
#define MATRIX_RAIN_LIMIT 1000

struct QuadData {
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
    GLuint glyphBuffer{}, glyphTexture{};
    GLuint ui_BaseColor{};
    GLuint vertexArray{}, vertexBuffer{};
    std::vector<QuadData> quadData;
};


#endif //MATRIX_H
