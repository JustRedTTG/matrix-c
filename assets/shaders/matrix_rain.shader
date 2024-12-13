#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
out vec4 fragPosition;
uniform float u_Time;
void main()
{
    if (u_Time < 0.0)
    {
        gl_Position = vec4(position, 1.0);
        return;
    }
    float angle = radians(u_Time * 360.0 / 10.0);
    mat4 rotation = mat4(
        cos(angle), -sin(angle), 0.0, 0.0,
        sin(angle), cos(angle), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    gl_Position = rotation * vec4(position, 1.0);
    fragPosition = gl_Position + vec4(u_Time, u_Time, 0.0, 0.0);
}

#shader fragment
#version 330 core
in vec4 fragPosition;
out vec4 color;
uniform float u_Time;
void main()
{
    if (u_Time < 0.0)
    {
        color = vec4(0.0, 0.0, 0.0, 0.1);
        return;
    }

    float zColor = abs(sin(u_Time)); // Vary the z component with time
    vec4 rawColor = vec4(fragPosition.x, fragPosition.y, zColor, 1.0);

    // Define the number of color levels
    int levels = 32;
    vec4 quantizedColor = floor(rawColor * float(levels)) / float(levels);

    color = quantizedColor;
}