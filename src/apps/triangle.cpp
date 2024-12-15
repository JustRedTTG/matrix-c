#include "apps/triangle.h"

static constexpr GLfloat triangleBufferData[] = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f
};

void TriangleApp::setup() {
    program = rnd->createProgram();
    rnd->loadShader(triangleShader, sizeof(triangleShader));
    rnd->useProgram();

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleBufferData), triangleBufferData, GL_STATIC_DRAW);

    glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(float),
            nullptr
        );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        reinterpret_cast<void *>(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    ui_Time = glGetUniformLocation(rnd->program, "u_Time");
}

void TriangleApp::loop() {
    rnd->useProgram(program);
    t += m * (rnd->clock->deltaTime / 10);

    if (m > 0 && t > 10) {
        m = -1;
    } else if (m < 0 && t < 0) {
        m = 1;
    }

    glUniform1f(ui_Time, t);

    rnd->clear();

    glBindVertexArray(vertexBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TriangleApp::destroy() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}