#version 330 core
layout(location = 0) in vec2 position;
uniform vec2 u_MousePosition;
uniform vec2 u_ScreenSize;

void main()
{
    vec2 mousePosition = u_MousePosition/(u_ScreenSize/2) - 1;
    vec4 finalPosition = vec4(
        position.x + mousePosition.x,
        position.y - mousePosition.y,
        0.0, 1.0
    );
    gl_Position = finalPosition;
}