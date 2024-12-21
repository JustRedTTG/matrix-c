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
uniform int u_Rotation;

out float v_ColorOffset;
flat out int v_Spark;
out vec2 v_TexCoord;

int generateRandomIndex(int instanceID, int maxIndex) {
    return abs(int(mod(floor(instanceID + u_Time * 100), float(maxIndex))));
}

void main()
{
    // Get a random index for the character data
    int randomIndex = generateRandomIndex(gl_InstanceID+1, u_MaxCharacters);
//    int randomIndex = gl_InstanceID;

    // Fetch the character data from the texture buffer using the random index
    CharacterInfo characterInfo = characterInfoList[randomIndex];

    float glyphXOffset = float(characterInfo.xOffset);
    float glyphYOffset = float(characterInfo.yOffset);
    float glyphWidth = float(characterInfo.width);
    float glyphHeight = float(characterInfo.height);

    float angle = radians(float(u_Rotation));
    mat2 rotationMatrix = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

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

    // Calculate the center of the character
    vec2 center = position + vec2(glyphWidth, glyphHeight) * 0.5 * u_CharacterScaling;

    // Translate to the center, apply rotation, and translate back
    screenPosition = rotationMatrix * (screenPosition - center) + center;

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
uniform float u_BaseColor;

in float v_ColorOffset;
flat in int v_Spark;
in vec2 v_TexCoord;

vec3 hueToRgb(float hue) {
    float r = abs(hue * 6.0 - 3.0) - 1.0;
    float g = 2.0 - abs(hue * 6.0 - 2.0);
    float b = 2.0 - abs(hue * 6.0 - 4.0);
    return clamp(vec3(r, g, b), 0.0, 1.0);
}

void main()
{
    float glyphColor = texture(u_AtlasTexture, v_TexCoord).r;

    vec3 color;

    if (v_Spark == 0) {
        color = vec3(1.0, 1.0, 1.0);
    } else {
        float hue = mod(u_BaseColor + v_ColorOffset, 1.0);
        color = hueToRgb(hue);
    }

    fragColor = vec4(color * glyphColor, glyphColor);
}