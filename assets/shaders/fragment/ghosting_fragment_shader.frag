#version 330 core

uniform sampler2D u_textureC;
uniform sampler2D u_textureP;
uniform float u_previousFrameOpacity;
in vec2 v_texcoord;
out vec4 fragColor;

void main() {
    vec4 currentFrame = texture(u_textureC, v_texcoord);
    vec4 previousFrame = texture(u_textureP, v_texcoord);

    vec4 blendedFrame = mix(previousFrame * u_previousFrameOpacity, currentFrame, currentFrame.a);

    // Proper alpha blending
    float alpha = currentFrame.a + (1.0 - currentFrame.a) * previousFrame.a * u_previousFrameOpacity;
    fragColor = vec4(blendedFrame.rgb * alpha, alpha);
    fragColor = clamp(fragColor, 0.0, 1.0);
}
