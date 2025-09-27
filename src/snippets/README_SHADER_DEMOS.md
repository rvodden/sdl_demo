# SDL_shadercross Shader Demonstrations

This directory contains cleaned-up demonstrations of SDL_shadercross functionality.

## Available Demonstrations

### 1. `shader_demo_success.cpp` - SDL_shadercross Library Test
**Purpose**: Demonstrates that SDL_shadercross is working correctly without visual rendering.

**Features**:
- ✅ SPIRV shader compilation via SDL_shadercross
- ✅ Shader reflection and metadata analysis
- ✅ Resource binding information
- ✅ SDL3 GPU device integration test
- ⚠️ No visual output (library functionality test only)

**Usage**:
```bash
./build/ninja-mc/bin/Debug/shader_demo_success
```

**Output**: Console confirmation that SDL_shadercross is working correctly.

### 2. `shader_demo_working.cpp` - Complete Visual Demonstration
**Purpose**: Complete working SDL_shadercross demonstration with visual output.

**Features**:
- ✅ SDL_shadercross SPIRV compilation
- ✅ Vulkan graphics pipeline creation
- ✅ Visual rendering (red triangle on dark blue background)
- ✅ Proper event handling (ESC to exit)
- ✅ Error-free execution

**Usage**:
```bash
./build/ninja-mc/bin/Debug/shader_demo_working
```

**Output**: Opens a window showing a red triangle rendered using SDL_shadercross compiled shaders.

### 3. `simple_test.cpp` - Basic SDL3 GPU Test
**Purpose**: Verify basic SDL3 GPU functionality without shaders.

**Features**:
- ✅ SDL3 GPU device creation
- ✅ Vulkan support verification
- ✅ Swapchain format detection
- ⚠️ No shader compilation or rendering

**Usage**:
```bash
./build/ninja-mc/bin/Debug/simple_test
```

**Output**: Console confirmation that SDL3 GPU is working.

## Technical Details

### Shader Source Files
The GLSL source code is located in the `shaders/` folder:
- **`shaders/triangle.vert`** - Vertex shader source (GLSL)
- **`shaders/triangle.frag`** - Fragment shader source (GLSL)
- **`shaders/CMakeLists.txt`** - CMake build configuration for shaders
- **`shaders/README.md`** - Detailed shader documentation

### CMake-Integrated SPIRV Compilation
All shader demos use SPIRV bytecode automatically compiled during the build process:
- **Vertex Shader**: Takes `vec2 position` input, outputs `gl_Position`
- **Fragment Shader**: Outputs solid red color `vec4(1.0, 0.0, 0.0, 1.0)`

**Automatic Build Process**:
1. CMake finds `glslang` compiler
2. Compiles GLSL to SPIRV bytecode (`.spv` files)
3. Generates C header files with SPIRV arrays
4. Validates SPIRV with `spirv-val` (if available)
5. Provides headers to C++ demos via `shader_headers` interface library

**Generated Headers** (in build directory):
- **`triangle_vert.h`** - Contains `triangleVertexSpirv[]` array
- **`triangle_frag.h`** - Contains `triangleFragmentSpirv[]` array

**Workflow**: Simply edit the GLSL files and rebuild - no manual compilation needed!

### Build Requirements
- SDL3 with Vulkan support
- SDL_shadercross library
- C++20 compiler
- Vulkan validation layers (for debugging)

### Resolved Issues
1. **SPIRV Interface Errors**: Fixed by using properly compiled SPIRV with correct interface declarations
2. **Pipeline Creation Failures**: Resolved by matching vertex attributes to shader inputs
3. **Validation Errors**: All Vulkan validation layer errors have been resolved

## Development Notes

This directory previously contained multiple experimental versions (`shader_demo.cpp`, `shader_demo_simple.cpp`, etc.) that have been cleaned up. The remaining files represent the final, working implementations.

For shader development workflow:
1. Start with `simple_test.cpp` to verify SDL3 GPU functionality
2. Use `shader_demo_success.cpp` to verify SDL_shadercross library integration
3. Use `shader_demo_working.cpp` for complete visual demonstrations

All shader demos can be built with:
```bash
cmake --build --preset ninja-mc-debug --target shader_demo_success
cmake --build --preset ninja-mc-debug --target shader_demo_working
cmake --build --preset ninja-mc-debug --target simple_test
```