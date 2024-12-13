#shader-vertex
#version 330 core
layout(location = 0) in vec3 position;
uniform float u_Time;
void main()
{
    float angle = radians(u_Time * 360.0);
    mat4 rotation = mat4(
        cos(angle), -sin(angle), 0.0, 0.0,
        sin(angle), cos(angle), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    gl_Position = rotation * vec4(position, 1.0);
}

#shader fragment
#version 330 core
out vec4 color;
uniform float u_Time;
void main()
{
    color = vec4(u_Time, 1.0-u_Time, 1.0, 1.0);
}