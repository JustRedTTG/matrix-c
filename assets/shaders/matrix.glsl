#shader vertex
#version 330 core
struct CharacterInfo {
    uint xOffset;
    uint yOffset;
    uint width;
    uint height;
};

layout(location = 0) in vec2 position;
layout(location = 1) in float colorOffset;
layout(location = 2) in int spark;
layout(std140) uniform u_AtlasBuffer {
    CharacterInfo characterInfoList[64];
};

uniform mat4 u_Projection;
uniform vec2 u_AtlasTextureSize;
uniform int u_MaxCharacters;
uniform float u_CharacterScaling;
uniform float u_Time;

out float v_ColorOffset;
flat out int v_Spark;
out vec2 v_TexCoord;

int generateRandomIndex(int instanceID, int maxIndex) {
    return abs(int(mod(floor(instanceID * u_Time), float(maxIndex))));
}

void main()
{
    // Get a random index for the character data
    int randomIndex = generateRandomIndex(gl_InstanceID, u_MaxCharacters);

    // Fetch the character data from the texture buffer using the random index
    CharacterInfo characterInfo = characterInfoList[randomIndex];

    float glyphXOffset = float(characterInfo.xOffset);
    float glyphYOffset = float(characterInfo.yOffset);
    float glyphWidth = float(characterInfo.width);
    float glyphHeight = float(characterInfo.height);

    int quadID = gl_InstanceID;

    // Calculate the position of the vertex
    vec2 vertexPosition = vec2(0.0);
    if (gl_VertexID == 0) {
        vertexPosition = vec2(0.0, 0.0);
    } else if (gl_VertexID == 1) {
        vertexPosition = vec2(glyphWidth, 0.0);
    } else if (gl_VertexID == 2) {
        vertexPosition = vec2(glyphWidth, glyphHeight);
    } else if (gl_VertexID == 3) {
        vertexPosition = vec2(0.0, glyphHeight);
    }

    // Add the vertex position in NDC
    vec2 screenPosition = position + (vertexPosition * u_CharacterScaling);
    vec2 atlasPosition = vec2(glyphXOffset, glyphYOffset) + vertexPosition;

    // Apply the projection matrix to get the position in clip space
    gl_Position = u_Projection * vec4(screenPosition, 0.0, 1.0);

    v_ColorOffset = colorOffset;
    v_Spark = spark;
    v_TexCoord = vec2(atlasPosition.x, u_AtlasTextureSize.y - atlasPosition.y) / u_AtlasTextureSize;
}

#shader fragment
#version 330 core
out vec4 fragColor;

uniform sampler2D u_AtlasTexture;
uniform vec3 u_BaseColor;

in float v_ColorOffset;
flat in int v_Spark;
in vec2 v_TexCoord;

void main()
{
    float glyphColor = texture(u_AtlasTexture, v_TexCoord).r;

    fragColor = vec4(u_BaseColor * glyphColor, glyphColor);
}