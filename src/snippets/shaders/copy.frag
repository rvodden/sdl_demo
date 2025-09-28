#version 450

// Simple copy shader - just displays the input texture
layout(location = 0) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D inputTexture;

void main() {
    outColor = texture(inputTexture, texCoords);
}