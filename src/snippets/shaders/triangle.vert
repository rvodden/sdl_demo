#version 450

// Vertex input: position from vertex buffer
layout(location = 0) in vec2 position;

void main() {
    // Transform 2D position to 4D clip space
    // Z = 0.0 (flat), W = 1.0 (no perspective)
    gl_Position = vec4(position, 0.0, 1.0);
}