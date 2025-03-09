#include "apps/matrix.h"
#include <fonts.h>
#include <gl_errors.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include "matrix_font.h"
#include "matrix_font_info.h"

void MatrixApp::setup() {
    rnd->opts->postProcessingOptions |= GHOSTING;
    rnd->opts->ghostingPreviousFrameOpacity = 0.997f;
    rnd->opts->ghostingBlurSize = 0.1f;
    // Handle font initialization
    atlas = createFontTextureAtlas(matrixFont, &matrixFontInfo);

    // Handle program initialization
    program = new ShaderProgram();
    program->loadShader(matrixShader, sizeof(matrixShader));
    program->useProgram();

    const float characterScale = static_cast<float>(rnd->opts->height) / (MATRIX_DEBUG ? 20.0 : 70.0) / static_cast<float>(matrixFontInfo.size);
    mouseRadius = rnd->opts->height / 10.0f;

    GL_CHECK(glUniform1i(program->getUniformLocation("u_AtlasTexture"), 0));
    GL_CHECK(glUniform1i(program->getUniformLocation("u_MaxCharacters"), matrixFontInfo.characterCount-1));
    GL_CHECK(glUniform1i(program->getUniformLocation("u_Rotation"), MATRIX_ROTATION));
    GL_CHECK(glUniform1f(program->getUniformLocation("u_CharacterScaling"), characterScale));
    GL_CHECK(glUniform2f(program->getUniformLocation("u_AtlasTextureSize"), atlas->atlasWidth, atlas->atlasHeight));

    const GLuint blockIndex = program->getUniformBlockIndex("u_AtlasBuffer");
    program->uniformBlockBinding(blockIndex, 0);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(rnd->opts->width), 0.0f,
                                      static_cast<float>(rnd->opts->height));
    GL_CHECK(glUniformMatrix4fv(program->getUniformLocation("u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));

    ui_BaseColor = program->getUniformLocation("u_BaseColor");
    ui_Time = program->getUniformLocation("u_Time");

    // Handle vertex buffer initialization
    rainDrawData.resize(MATRIX_RAIN_LIMIT);
    rainData.resize(MATRIX_RAIN_LIMIT);

    GL_CHECK(glGenVertexArrays(1, &vertexArray));
    GL_CHECK(glBindVertexArray(vertexArray));

    GL_CHECK(glGenBuffers(1, &vertexBuffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(
        glBufferData(GL_ARRAY_BUFFER, MATRIX_RAIN_LIMIT * sizeof(RainDrawData), nullptr,
            GL_STREAM_DRAW));

    GL_CHECK(glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RainDrawData),
        nullptr
    ));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(RainDrawData),
        reinterpret_cast<void *>(2 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(
        2,
        1,
        GL_INT,
        GL_FALSE,
        sizeof(RainDrawData),
        reinterpret_cast<void *>(3 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(2));

    GL_CHECK(glVertexAttribDivisor(0, 1));
    GL_CHECK(glVertexAttribDivisor(1, 1));
    GL_CHECK(glVertexAttribDivisor(2, 1));

    // Initialize vertices
    for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
        if constexpr (MATRIX_DEBUG) {
            rainDrawData[i].x = matrixFontInfo.characterInfoList[i].xOffset * characterScale;
            rainDrawData[i].y = matrixFontInfo.characterInfoList[i].yOffset * characterScale;
        } else {
            rainDrawData[i].x = random_td_float(0, rnd->opts->width);
            rainDrawData[i].y = random_td_float(0, rnd->opts->height);
        }
        rainDrawData[i].colorOffset = randomColorOffset();
        rainDrawData[i].spark = randomSpark();
        rainData[i].speed = randomSpeed();
        if constexpr (MATRIX_DEBUG) {
            rainData[i].speed = 0;
        } else if constexpr (MATRIX_UP) {
            rainData[i].speed *= -1;
        }
    }
}

void MatrixApp::loop() {
    program->useProgram();

    // Bind glyph buffer and texture
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, atlas->glyphTexture));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, atlas->glyphBuffer);

    GL_CHECK(glUniform1f(ui_BaseColor, baseColor));
    GL_CHECK(glUniform1f(ui_Time, rnd->clock->floatTime()));

    baseColor += rnd->clock->deltaTime / MATRIX_DELTA_MULTIPLIER;

    int amountOfReassignedRaindrops = std::max(0, static_cast<int>(rnd->events->keysPressed) * MATRIX_EFFECT_PER_KEYPRESS);
    if (rnd->events->mouseLeft) {
        amountOfReassignedRaindrops += MATRIX_DRAW_STRENGTH;
    }

    if (amountOfReassignedRaindrops > MATRIX_RAIN_LIMIT - activeCursorPardons) {
        amountOfReassignedRaindrops = MATRIX_RAIN_LIMIT - activeCursorPardons;
    }

    const int reassignedRaindrop = amountOfReassignedRaindrops > 0 ? random_int(0, MATRIX_RAIN_LIMIT - 1) : -1;

    int activeCursorPardons = 0;
    for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
        incrementRain(i, i == reassignedRaindrop);
        if (rainData[i].cursorPardons > 0) {
            activeCursorPardons++;
        }
    }

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, rainDrawData.size() * sizeof(RainDrawData), rainDrawData.data()));

    // Render
    GL_CHECK(glBindVertexArray(vertexArray));
    GL_CHECK(glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, MATRIX_RAIN_LIMIT));
    // rnd->fboPTextureOutput = atlas->glyphTexture;
}

void MatrixApp::destroy() {
    atlas->destroy();
    GL_CHECK(glDeleteBuffers(1, &vertexBuffer));
    GL_CHECK(glDeleteVertexArrays(1, &vertexArray));
    program->destroy();
}

int MatrixApp::random_int(const int a, const int b) {
    return a + rand() % ((b+1) - a);
}

int MatrixApp::random_td_int(int a, int b)
{
    a /= MATRIX_TEXT_SIZE_DIVISOR;
    b /= MATRIX_TEXT_SIZE_DIVISOR;
    return random_int(a, b) * MATRIX_TEXT_SIZE_DIVISOR;
}

float MatrixApp::random_float(const float a, const float b) {
    return a + static_cast<float>(rand()) / RAND_MAX * (b - a);
}

float MatrixApp::random_td_float(float a, float b) {
    a /= MATRIX_TEXT_SIZE_DIVISOR;
    b /= MATRIX_TEXT_SIZE_DIVISOR;
    return random_float(a, b) * MATRIX_TEXT_SIZE_DIVISOR;
}

int MatrixApp::randomMultiplier() {
    return random_int(0, 1) == 0 ? -1 : 1;
}

int MatrixApp::randomSpark() {
    return random_int(0, MATRIX_CHANCE_OF_SPARK);
}

int MatrixApp::randomSpeed() {
    return random_td_float(10, 20);
}

float MatrixApp::randomColorOffset() {
    return random_float(-MATRIX_COLOR_VARIATION, MATRIX_COLOR_VARIATION);
}

void MatrixApp::resetRain(const int index) {
    rainDrawData[index].x = random_td_float(0, rnd->opts->width);
    rainDrawData[index].spark = randomSpark();
    rainDrawData[index].colorOffset = randomColorOffset();
    rainData[index].speed = randomSpeed();
}

void MatrixApp::incrementRain(const int index, const bool reassigned) {
    // Move the raindrop along its path
    const float addX = rot_d15_m2 * randomMultiplier();
    const float speed = rainData[index].speed - rot_d15_d2;

    float mouseX = static_cast<float>(rnd->events->mouseX);
    float mouseY = static_cast<float>(rnd->opts->height - rnd->events->mouseY);
    float dx = rainDrawData[index].x - mouseX;
    float dy = rainDrawData[index].y - mouseY;
    float distance = sqrt(dx * dx + dy * dy);

    if (reassigned) {
        rainData[index].cursorPardons = rnd->events->mouseLeft ? 300 : 100;
        rainDrawData[index].colorOffset += 0.5;
        rainDrawData[index].x = mouseX;
        rainDrawData[index].y = mouseY;
        if (random_int(0, 4) == 0) {
            rainData[index].pushX = cos(random_int(0, 360) * M_PI / 180.0f) * rnd->clock->deltaTime * MATRIX_DELTA_MULTIPLIER * MATRIX_SPEED_DRAW;
            rainData[index].pushY = sin(random_int(0, 360) * M_PI / 180.0f) * rnd->clock->deltaTime *  MATRIX_DELTA_MULTIPLIER * MATRIX_SPEED_DRAW;
        } else {
            rainData[index].pushX = 0;
            rainData[index].pushY = 0;
        }
    }

    if (rainData[index].cursorPardons == 0 and distance < mouseRadius and random_int(0, 10) != 0) {
        // Push the raindrop away from the cursor
        const float force = (mouseRadius - distance) / mouseRadius;
        const float pushX = dx / distance * force * 100.0f; // Adjust the push strength as needed
        const float pushY = dy / distance * force * 100.0f;

        rainDrawData[index].x += pushX;
        rainData[index].pushX += pushX;
        rainData[index].pushY += pushY;
        rainDrawData[index].y += pushY;
    } else if (rainData[index].cursorPardons > 0) {
        // Expand from the cursor
        rainDrawData[index].x += rainData[index].pushX;
        rainDrawData[index].y += rainData[index].pushY;

        rainData[index].cursorPardons--;
        if (rainData[index].cursorPardons == 0) {
            rainData[index].pushX = 0;
            rainData[index].pushY = 0;
        }
    } else if (rainData[index].pushX != 0 || rainData[index].pushY != 0) {
        // Reset the push force
        rainDrawData[index].x -= rainData[index].pushX;
        rainDrawData[index].y -= rainData[index].pushY;
        rainData[index].pushX = 0;
        rainData[index].pushY = 0;
    }

    if (rainData[index].cursorPardons == 0) {
        rainDrawData[index].x += addX;
        rainDrawData[index].y -= speed * rnd->clock->deltaTime * MATRIX_DELTA_MULTIPLIER;
    }

    if (random_int(0, 1000) == 0) {
        rainDrawData[index].x = random_td_float(0, rnd->opts->width);
    }


    // Finally check the position of the raindrop to see if it needs to be reset
    if constexpr (MATRIX_UP && rainDrawData[index].y >= rnd->opts->height) {
        rainDrawData[index].y = 0;
        resetRain(index);
    } else if (rainDrawData[index].y < 0) {
        rainDrawData[index].y = static_cast<float>(rnd->opts->height);
        resetRain(index);
    }
}
