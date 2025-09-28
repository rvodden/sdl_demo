#version 450

// Bright pass filter - extracts bright areas for bloom
layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D sceneTexture;

void main() {
    vec3 color = texture(sceneTexture, texCoords).rgb;

    // Calculate luminance using standard weights
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));

    // Threshold for bright areas (adjust this value to control bloom)
    float threshold = 0.8;

    // Extract bright areas
    if (luminance > threshold) {
        // Scale the brightness to create bloom intensity
        float bloomStrength = (luminance - threshold) / (1.0 - threshold);
        outColor = vec4(color * bloomStrength, 1.0);
    } else {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}