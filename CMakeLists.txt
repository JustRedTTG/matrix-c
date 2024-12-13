cmake_minimum_required(VERSION 3.30)
project(matrix)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

include_directories("include")


add_executable(matrix
        src/main.cpp
        src/shader.cpp
        src/options.cpp
        src/renderer.cpp
)

target_link_libraries(
        matrix
        ${OPENGL_LIBRARIES} GL GLEW glfw
)

if (UNIX)  # This includes Linux and other Unix-like systems
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_sources(matrix PRIVATE src/x11.cpp)
    target_link_libraries(matrix ${X11_LIBRARIES} X11 Xrender)
endif ()