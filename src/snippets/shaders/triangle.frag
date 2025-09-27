#version 450

// Fragment output: color to framebuffer
layout(location = 0) out vec4 outColor;

void main() {
    // Output solid red color (RGB = 1,0,0, Alpha = 1)
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}