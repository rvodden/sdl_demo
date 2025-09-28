#version 450

// Scene fragment shader - renders white rectangle
layout(location = 0) out vec4 outColor;

void main() {
    // Output bright white color
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}