#version 450

// Gaussian blur shader for bloom effect
layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D inputTexture;

// Storage buffer for blur parameters
layout(binding = 1) readonly buffer BlurParams {
    vec2 direction; // (1,0) for horizontal, (0,1) for vertical
    float strength; // blur strength multiplier
} blurParams;

void main() {
    vec2 texelSize = 1.0 / textureSize(inputTexture, 0);
    vec2 offset = blurParams.direction * texelSize * blurParams.strength;

    // 5-tap Gaussian blur
    vec3 color = vec3(0.0);

    // Gaussian weights for 5 samples
    float weights[5] = float[](0.06136, 0.24477, 0.38774, 0.24477, 0.06136);

    // Sample along blur direction
    for (int i = -2; i <= 2; i++) {
        vec2 sampleCoord = texCoords + float(i) * offset;
        color += texture(inputTexture, sampleCoord).rgb * weights[i + 2];
    }

    outColor = vec4(color, 1.0);
}