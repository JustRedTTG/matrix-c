#ifndef MATRIX_H
#define MATRIX_H
#include <apps.h>
#include <fonts.h>
#include "matrix_shader.h"

#include "glad.h"

// #define MATRIX_CHARACTERS "ﾊﾐﾋｰｳｼﾅﾓﾆｻﾜﾂｵﾘｱﾎﾃﾏｹﾒｴｶｷﾑﾕﾗｾﾈｽﾀﾇﾍ012345789Z:・.\"=*+-<>¦｜"
#define MATRIX_CHARACTERS "3"
#define MATRIX_RAIN_LIMIT 1000
#define MATRIX_TEXT_SIZE_DIVISOR 2.0
#define MATRIX_CHANCE_OF_SPARK 5

struct RainDrawData {
    float x, y;
    float colorOffset;
    int spark;
};

struct RainData {
    float speed;
};

class MatrixApp final : public App {
public:
    explicit MatrixApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
private:
    ShaderProgram *program{};
    FontAtlas *atlas{};
    GLuint ui_BaseColor{}, ui_Time{};
    GLuint vertexArray{}, vertexBuffer{};
    std::vector<RainDrawData> quadData;
};


#endif //MATRIX_H
