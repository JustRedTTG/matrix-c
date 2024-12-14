#include "apps/debug.h"
#include "cursor_motion_vertex_shader.h"
#include "debug_fragment_shader.h"
#include <helper.h>

void DebugApp::setup() {
    createQuadVertexData(rnd, 50.0, 50.0, vertices);

    program = rnd->createProgram();
    rnd->loadShaderInternal(debugFragmentShader, sizeof(debugFragmentShader), GL_FRAGMENT_SHADER);
    rnd->loadShaderInternal(cursorMotionVertexShader, sizeof(cursorMotionVertexShader), GL_VERTEX_SHADER);
    rnd->linkProgram();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLuint indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ui_MousePosition = glGetUniformLocation(rnd->program, "u_MousePosition");

}

void DebugApp::loop() {
    rnd->useProgram();
    GLuint ui_ScreenSize = glGetUniformLocation(rnd->program, "u_ScreenSize");
    glUniform2f(ui_ScreenSize, static_cast<GLfloat>(rnd->opts->width), static_cast<GLfloat>(rnd->opts->height));
    glUniform2f(ui_MousePosition, static_cast<GLfloat>(rnd->events->mouseX), static_cast<GLfloat>(rnd->events->mouseY));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr
    );
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glDisableVertexAttribArray(0);
}

void DebugApp::destroy() {
    glDeleteBuffers(1, &vertexBuffer);
}