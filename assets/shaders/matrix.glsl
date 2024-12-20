#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float colorOffset;
layout(location = 2) in int spark;

uniform mat4 u_Projection;
uniform samplerBuffer u_AtlasBuffer;

out float v_ColorOffset;
out int v_Spark;

void main()
{
    v_ColorOffset = colorOffset;
    v_Spark = spark;

    gl_Position = u_Projection * position;
    gl_PointSize = 20.0;
}

#shader fragment
#version 330 core
out vec4 fragColor;

uniform sampler2D u_AtlasTexture;
uniform vec3 u_BaseColor;

void main()
{
    fragColor = vec4(u_BaseColor, 1.0);
}