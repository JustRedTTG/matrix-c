#include "apps/matrix.h"
#include <fonts.h>
#include <gl_errors.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrix_font.h"

void MatrixApp::setup() {
    rnd->opts->postProcessingOptions |= GHOSTING;
    // Handle font initialization
    initFonts();
    font = loadFont(matrixFont, sizeof(matrixFont));
    setFontSize(font, 0, rnd->opts->height / 80); // 80 characters per line
    auto [glyphMatrixTexture, glyphBuffer, atlasWidth, atlasHeight] = createFontTextureAtlas(font, MATRIX_CHARACTERS);
    this->glyphTexture = glyphMatrixTexture;
    this->glyphBuffer = glyphBuffer;

    // Handle program initialization
    program = new ShaderProgram();
    program->loadShader(matrixShader, sizeof(matrixShader));
    program->useProgram();

    GL_CHECK(glUniform1i(program->getUniformLocation("u_AtlasTexture"), 0));
    GL_CHECK(glUniform1i(program->getUniformLocation("u_MaxCharacters"), sizeof(MATRIX_CHARACTERS) - 1));
    GL_CHECK(glUniform2f(program->getUniformLocation("u_AtlasTextureSize"), atlasWidth, atlasHeight));
    std::cout << "Atlas size: " << atlasWidth << "x" << atlasHeight << std::endl;
    const GLuint blockIndex = program->getUniformBlockIndex("u_AtlasBuffer");
    program->uniformBlockBinding(blockIndex, 0);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(rnd->opts->width), 0.0f,
                                      static_cast<float>(rnd->opts->height));
    GL_CHECK(glUniformMatrix4fv(program->getUniformLocation("u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));

    ui_BaseColor = program->getUniformLocation("u_BaseColor");

    // Handle vertex buffer initialization
    quadData.resize(MATRIX_RAIN_LIMIT);

    GL_CHECK(glGenVertexArrays(1, &vertexArray));
    GL_CHECK(glBindVertexArray(vertexArray));

    GL_CHECK(glGenBuffers(1, &vertexBuffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(
        glBufferData(GL_ARRAY_BUFFER, MATRIX_RAIN_LIMIT * sizeof(QuadData), nullptr,
            GL_STREAM_DRAW));

    GL_CHECK(glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(QuadData),
        nullptr
    ));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(QuadData),
        reinterpret_cast<void *>(2 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(
        2,
        1,
        GL_INT,
        GL_TRUE,
        sizeof(QuadData),
        reinterpret_cast<void *>(3 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(2));

    GL_CHECK(glVertexAttribDivisor(0, 1));
    GL_CHECK(glVertexAttribDivisor(1, 1));
    GL_CHECK(glVertexAttribDivisor(2, 1));

    // Initialize vertices
    for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
        quadData[i].x = static_cast<float>(rand() % rnd->opts->width);
        quadData[i].y = static_cast<float>(rand() % rnd->opts->height);
        quadData[i].colorOffset = static_cast<float>(rand()) / RAND_MAX;
        quadData[i].spark = static_cast<bool>(rand() % 2);
    }
}

void MatrixApp::loop() {
    program->useProgram();

    // Bind glyph buffer and texture
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, glyphTexture));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, glyphBuffer);

    GL_CHECK(glUniform3f(ui_BaseColor, 1.0f, 1.0f, 1.0f));

    // for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
    //     vertices[i].x = static_cast<float>(rand() % rnd->opts->width);
    //     vertices[i].y = static_cast<float>(rand() % rnd->opts->height);
    //     vertices[i].colorOffset = static_cast<float>(rand()) / RAND_MAX;
    //     vertices[i].spark = static_cast<bool>(rand() % 2);
    // }

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, quadData.size() * sizeof(QuadData), quadData.data()));

    // Render
    GL_CHECK(glBindVertexArray(vertexArray));
    GL_CHECK(glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, MATRIX_RAIN_LIMIT));
    // rnd->fboPTextureOutput = glyphTexture;
}

void MatrixApp::destroy() {
    FT_Done_Face(font);
    GL_CHECK(glDeleteBuffers(1, &glyphBuffer));
    GL_CHECK(glDeleteTextures(1, &glyphTexture));
    GL_CHECK(glDeleteBuffers(1, &vertexBuffer));
    GL_CHECK(glDeleteVertexArrays(1, &vertexArray));
    program->destroy();
}
