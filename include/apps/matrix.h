#ifndef MATRIX_H
#define MATRIX_H
#include <apps.h>
#include <fonts.h>
#include "matrix_shader.h"

#include "glad.h"

#define MATRIX_RAIN_LIMIT 1000
#define MATRIX_DELTA_MULTIPLIER 20
#define MATRIX_COLOR_VARIATION 0.05
#define MATRIX_TEXT_SIZE_DIVISOR 2.0
#define MATRIX_CHANCE_OF_SPARK 5
#define MATRIX_EFFECT_PER_KEYPRESS 10
#define MATRIX_DRAW_STRENGTH 100
#define MATRIX_ROTATION 5
#define MATRIX_DEBUG false
#define MATRIX_UP false

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct RainDrawData {
    float x, y;
    float colorOffset;
    int spark;
};

struct RainData {
    float speed = 0;
    float pushX, pushY = 0;
    int cursorPardons = 0;
};

class MatrixApp final : public App {
public:
    explicit MatrixApp(renderer *rnd) : App(rnd) {};
    void setup() override;
    void loop() override;
    void destroy() override;
private:
    static int random_int(int a, int b);
    static int random_td_int(int a, int b);
    static float random_float(float a, float b);
    static float random_td_float(float a, float b);
    static int randomMultiplier();
    static int randomSpark();
    static int randomSpeed();
    static float randomColorOffset();
    void resetRain(int index);
    void incrementRain(int index, bool reassigned);

    ShaderProgram *program{};
    FontAtlas *atlas{};
    GLuint ui_BaseColor{}, ui_Time{};
    GLuint vertexArray{}, vertexBuffer{};
    std::vector<RainDrawData> rainDrawData;
    std::vector<RainData> rainData;
    float baseColor = 0.0f;
    float mouseRadius = 0.0f;
    int activeCursorPardons = 0;
    const float rot_d15 = MATRIX_ROTATION / 15.0;
    const float rot_d15_m2 = rot_d15 * 2;
    const float rot_d15_d2 = rot_d15 / 2;
};

#endif //MATRIX_H
