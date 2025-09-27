# Shader Source Files

This directory contains the GLSL source code for shaders used in the SDL_shadercross demonstrations.

## Shader Files

### `triangle.vert` - Vertex Shader
**Purpose**: Transforms 2D vertex positions to clip space for triangle rendering.

**Inputs**:
- `layout(location = 0) in vec2 position` - 2D vertex position from vertex buffer

**Outputs**:
- `gl_Position` - 4D clip space position (x, y, 0.0, 1.0)

**Description**: Simple pass-through vertex shader that takes 2D coordinates and converts them to 4D clip space coordinates suitable for rasterization.

### `triangle.frag` - Fragment Shader
**Purpose**: Outputs solid red color for each pixel.

**Inputs**: None (could be extended with texture coordinates, colors, etc.)

**Outputs**:
- `layout(location = 0) out vec4 outColor` - RGBA color output

**Description**: Simple fragment shader that outputs a solid red color (1.0, 0.0, 0.0, 1.0) for every pixel.

## CMake-Based SPIRV Compilation

The GLSL source files are automatically compiled to SPIRV bytecode using CMake and `glslang`. The compilation happens during the build process and generates C header files that are included by the demos.

### Build Process

When you build the project, CMake automatically:

1. **Finds glslang**: Locates the `glslang` compiler
2. **Compiles GLSL → SPIRV**: Converts `.vert` and `.frag` files to `.spv` bytecode
3. **Generates C headers**: Creates `.h` files with SPIRV arrays
4. **Validates SPIRV**: Runs `spirv-val` if available
5. **Provides includes**: Makes headers available to C++ demos

### Generated Files

The CMake build generates these files in the build directory:
- `triangle.vert.spv` - SPIRV binary for vertex shader
- `triangle.frag.spv` - SPIRV binary for fragment shader
- `triangle_vert.h` - C header with `triangleVertexSpirv[]` array
- `triangle_frag.h` - C header with `triangleFragmentSpirv[]` array

### CMake Targets

- `cmake --build --preset ninja-mc-debug --target compile_shaders` - Compile just the shaders
- `cmake --build --preset ninja-mc-debug --target validate_shaders` - Validate SPIRV (if spirv-val available)

## Development Workflow

To modify the shaders:

1. **Edit GLSL source**: Modify `.vert` or `.frag` files in this directory
2. **Rebuild**: Run `cmake --build --preset ninja-mc-debug` - shaders will be recompiled automatically
3. **Test**: The updated shaders will be included in the demos

**No manual compilation needed!** - Just edit the GLSL files and rebuild.

### Example Extensions:
- **Colored triangles**: Add color input to vertex shader, pass to fragment shader
- **Textured triangles**: Add texture coordinates and texture sampling
- **Animated effects**: Add uniform buffers with time/transformation data
- **Multiple shapes**: Create additional shader variants for different primitives

## Vulkan/SPIRV Requirements

- **GLSL Version**: `#version 450` (Vulkan-compatible GLSL)
- **Explicit Locations**: All inputs/outputs must have explicit `layout(location = N)`
- **SPIRV Target**: Compiled SPIRV must be compatible with Vulkan 1.0+
- **Interface Matching**: Vertex shader outputs must match pipeline vertex input descriptions