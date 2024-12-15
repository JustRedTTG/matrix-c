cmake_minimum_required(VERSION 3.30)
project(matrix)

set(CMAKE_CXX_STANDARD 20)
set(RC_DEPENDS "")

file(MAKE_DIRECTORY "generated")

function(embed_resource resource_file_name source_file_name variable_name)

    file(READ ${resource_file_name} hex_content HEX)

    string(REPEAT "[0-9a-f]" 32 column_pattern)
    string(REGEX REPLACE "(${column_pattern})" "\\1\n" content "${hex_content}")

    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

    string(REGEX REPLACE ", $" "" content "${content}")

    set(array_definition "#pragma once\nstatic constexpr unsigned char ${variable_name}[] =\n{\n${content}\n};")

    set(source "// Auto generated file.\n${array_definition}\n")

    file(WRITE "${source_file_name}" "${source}")

endfunction()

find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS chrono)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

include_directories("include")
include_directories("generated")


# Embed files
embed_resource("assets/help_message.txt" "generated/help_message.h" "helpMessage")
embed_resource("assets/apps_message.txt" "generated/apps_message.h" "appsMessage")
embed_resource("assets/shaders/triangle.glsl" "generated/triangle_shader.h" "triangleShader")
embed_resource("assets/shaders/vertex/cursor_motion.vert" "generated/cursor_motion_vertex_shader.h" "cursorMotionVertexShader")
embed_resource("assets/shaders/fragment/debug.frag" "generated/debug_fragment_shader.h" "debugFragmentShader")
embed_resource("assets/shaders/fragment/basic_texture_fragment_shader.frag" "generated/basic_texture_fragment_shader.h" "basicTextureFragmentShader")
embed_resource("assets/shaders/vertex/basic_texture_vertex_shader.vert" "generated/basic_texture_vertex_shader.h" "basicTextureVertexShader")
embed_resource("assets/shaders/fragment/ghosting_fragment_shader.frag" "generated/ghosting_fragment_shader.h" "ghostingFragmentShader")
embed_resource("assets/shaders/fragment/blur_fragment_shader.frag" "generated/blur_fragment_shader.h" "blurFragmentShader")

add_executable(matrix
        src/main.cpp
        src/shader.cpp
        src/options.cpp
        src/renderer.cpp
        src/events.cpp
        src/clock.cpp
        src/helper.cpp
        src/gl_errors.cpp
        src/apps/triangle.cpp
        src/apps.cpp
        src/apps/matrix.cpp
        src/apps/debug.cpp
)

target_link_libraries(
        matrix
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        glfw
        ${Boost_LIBRARIES}
)

if (UNIX)  # This includes Linux and other Unix-like systems
    find_package(X11 REQUIRED)
    include_directories("include-linux")
    include_directories(${X11_INCLUDE_DIR})
    target_sources(matrix PRIVATE src/x11.cpp)
    target_link_libraries(matrix ${X11_LIBRARIES} X11 Xrender Xi)
endif ()