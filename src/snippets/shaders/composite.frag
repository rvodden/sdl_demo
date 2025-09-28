#version 450

// Final composite shader - combines original scene with bloom
layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D sceneTexture;
layout(binding = 1) uniform sampler2D bloomTexture;

void main() {
    vec3 scene = texture(sceneTexture, texCoords).rgb;
    vec3 bloom = texture(bloomTexture, texCoords).rgb;

    // Add yellow tint to bloom
    bloom *= vec3(1.0, 1.0, 0.3); // Yellow bloom effect

    // Combine scene and bloom with additive blending
    vec3 finalColor = scene + bloom * 2.0; // Multiply bloom intensity

    outColor = vec4(finalColor, 1.0);
}