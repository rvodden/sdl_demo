#version 450

// Fullscreen quad vertex shader
layout(location = 0) in vec2 position;
layout(location = 0) out vec2 texCoords;

void main() {
    // Position is already in NDC space (-1 to 1)
    gl_Position = vec4(position, 0.0, 1.0);

    // Convert position to texture coordinates (0 to 1)
    texCoords = position * 0.5 + 0.5;
}