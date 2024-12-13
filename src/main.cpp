#include <chrono>

#include "shader.h"
#include "renderer.h"
#include <iostream>
#include <options.h>
#include <thread>
#include "shader_matrix_rain.h"

#ifdef __linux__
#include "x11.h"
#endif

int main(const int argc, char *argv[]) {
    options *opts = parseOptions(argc, argv);
    auto *rnd = new renderer(opts);

    rnd->makeWindow();

    rnd->createProgram();
    rnd->loadShader(matrixRainShader, sizeof(matrixRainShader));
    rnd->useProgram();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f,
     };
    static const GLfloat g_background_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
     };
    GLuint vertexBuffer;
    GLuint backgroundBuffer;

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &backgroundBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_background_buffer_data), g_background_buffer_data, GL_STATIC_DRAW);

    GLuint l_Time = glGetUniformLocation(rnd->program, "u_Time");

    float t = 0;
    float m = 1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    while (true) {
        rnd->getEvents();
        if (rnd->events->quit) {
            break;
        }

        t += (rnd->clock->deltaTime / 10) * m;

        if (m > 0 && t > 1) {
            m = -1;
            // glClear(GL_COLOR_BUFFER_BIT);
        } else if (m < 0 && t < 0) {
            m = 1;
        }

        glUniform1f(l_Time, t);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(
           0,
           3,
           GL_FLOAT,
           GL_FALSE,
           0,
           nullptr
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(1);

        glUniform1f(l_Time, -1.0f);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundBuffer);
        glVertexAttribPointer(
           0,
           3,
           GL_FLOAT,
           GL_FALSE,
           0,
           nullptr
        );
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        rnd->swapBuffers();
    }

    rnd->destroy();
    return 0;
}
