#include "apps/matrix.h"
#include <fonts.h>
#include <gl_errors.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrix_font.h"

void MatrixApp::setup() {
    // Handle font initialization
    initFonts();
    font = loadFont(matrixFont, sizeof(matrixFont));
    auto [glyphMatrixTexture, glyphMatrixBuffer] = createFontTextureAtlas(font, MATRIX_CHARACTERS);
    this->glyphMatrixTexture = glyphMatrixTexture;
    this->glyphMatrixBuffer = glyphMatrixBuffer;

    // Handle program initialization
    program = new ShaderProgram();
    program->loadShader(matrixShader, sizeof(matrixShader));
    program->useProgram();

    GL_CHECK(glUniform1i(program->getUniformLocation("u_AtlasBuffer"), 0));
    GL_CHECK(glUniform1i(program->getUniformLocation("u_AtlasTexture"), 1));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(rnd->opts->width), 0.0f,
                                      static_cast<float>(rnd->opts->height));
    GL_CHECK(glUniformMatrix4fv(program->getUniformLocation("u_Projection"), 1, GL_FALSE, glm::value_ptr(projection)));

    ui_BaseColor = program->getUniformLocation("u_BaseColor");

    // Handle vertex buffer initialization
    vertices.resize(MATRIX_RAIN_LIMIT);
    constexpr int stride = sizeof(GLfloat) * 3 + sizeof(GLint);

    GL_CHECK(glGenVertexArrays(1, &vertexArray));
    GL_CHECK(glBindVertexArray(vertexArray));

    GL_CHECK(glGenBuffers(1, &vertexBuffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(
        glBufferData(GL_ARRAY_BUFFER, MATRIX_RAIN_LIMIT * stride, nullptr,
            GL_STREAM_DRAW));

    GL_CHECK(glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride,
        nullptr
    ));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(
        1,
        1,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<void *>(2 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(
        2,
        1,
        GL_INT,
        GL_TRUE,
        stride,
        reinterpret_cast<void *>(3 * sizeof(float))
    ));
    GL_CHECK(glEnableVertexAttribArray(2));

    // Initialize vertices
    for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
        vertices[i].x = static_cast<float>(rand() % rnd->opts->width);
        vertices[i].y = static_cast<float>(rand() % rnd->opts->height);
        vertices[i].colorOffset = static_cast<float>(rand()) / RAND_MAX;
        vertices[i].spark = static_cast<bool>(rand() % 2);
    }
}

void MatrixApp::loop() {
    program->useProgram();

    // Bind glyph buffer and texture
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_BUFFER, glyphMatrixBuffer));

    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, glyphMatrixTexture));

    GL_CHECK(glUniform3f(ui_BaseColor, 0.0f, 1.0f, 0.0f));

    // for (int i = 0; i < MATRIX_RAIN_LIMIT; ++i) {
    //     vertices[i].x = static_cast<float>(rand() % rnd->opts->width);
    //     vertices[i].y = static_cast<float>(rand() % rnd->opts->height);
    //     vertices[i].colorOffset = static_cast<float>(rand()) / RAND_MAX;
    //     vertices[i].spark = static_cast<bool>(rand() % 2);
    // }

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));

    // Render
    GL_CHECK(glBindVertexArray(vertexArray));
    GL_CHECK(glDrawArrays(GL_POINTS, 0, MATRIX_RAIN_LIMIT));
}

void MatrixApp::destroy() {
    FT_Done_Face(font);
    GL_CHECK(glDeleteBuffers(1, &glyphMatrixBuffer));
    GL_CHECK(glDeleteTextures(1, &glyphMatrixTexture));
    program->destroy();
}
