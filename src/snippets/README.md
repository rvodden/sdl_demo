# SDL3 C++ Wrapper Snippets

This directory contains code examples and demonstrations for the SDL3 C++ wrapper library. Each snippet demonstrates specific features and usage patterns.

## Quick Start

Build all snippets:
```bash
cmake --build --preset ninja-mc-debug
```

Run any snippet:
```bash
./build/ninja-mc/bin/Debug/<snippet_name>
```

## Available Snippets

### Graphics and Rendering

#### `shader_demo` - Complete SDL_shadercross Visual Demo
**Purpose**: Complete working SDL_shadercross demonstration with visual output.

**Features**:
- ✅ SDL_shadercross SPIRV compilation
- ✅ Vulkan graphics pipeline creation
- ✅ Visual rendering (red triangle on dark blue background)
- ✅ Proper event handling (ESC to exit)

**Usage**:
```bash
./build/ninja-mc/bin/Debug/shader_demo
```

**Output**: Opens a window showing a red triangle rendered using SDL_shadercross compiled shaders.

#### `gpu_demo` - Basic SDL3 GPU Test
**Purpose**: Verify basic SDL3 GPU functionality without shaders.

**Features**:
- ✅ SDL3 GPU device creation
- ✅ Vulkan support verification
- ✅ Swapchain format detection
- ⚠️ No shader compilation or rendering

**Usage**:
```bash
./build/ninja-mc/bin/Debug/gpu_demo
```

**Output**: Console confirmation that SDL3 GPU is working.

#### `font_hello_world` - TTF Font Rendering Demo
**Purpose**: Demonstrates text rendering using SDL3_ttf integration.

**Features**:
- ✅ TTF font loading and rendering
- ✅ Service registry pattern usage
- ✅ BaseApplication framework
- ✅ Text rendering to texture

**Usage**:
```bash
./build/ninja-mc/bin/Debug/font_hello_world
```

**Output**: Window displaying "Hello, World!" text using TTF fonts.

### Event System Examples

#### `custom_events_example` - Custom User Events
**Purpose**: Demonstrates creating and handling custom user events.

**Features**:
- ✅ Custom event class definition (ScoreUpdateEvent)
- ✅ Event inheritance from CustomUserEvent
- ✅ Event dispatcher registration
- ✅ Type-safe event handling

**Usage**:
```bash
./build/ninja-mc/bin/Debug/custom_events_example
```

**Key Concepts**: Shows how to extend the event system with application-specific events.

#### `event_adaptor_example` - Decoupled Event Architecture
**Purpose**: Demonstrates the template-based EventAdaptor system.

**Features**:
- ✅ EventAdaptor zero-cost abstraction
- ✅ TemplatedEventBus with callback routing
- ✅ EventRouter callback-based usage
- ✅ Complete event processing decoupling

**Usage**:
```bash
./build/ninja-mc/bin/Debug/event_adaptor_example
```

**Key Concepts**: Advanced event system architecture for high-performance applications.

#### `keyboard_event_example` - Keyboard Input Handling
**Purpose**: Comprehensive keyboard event handling demonstration.

**Features**:
- ✅ Real-time keyboard input processing
- ✅ Key state tracking
- ✅ Text input handling
- ✅ Visual feedback display

**Usage**:
```bash
./build/ninja-mc/bin/Debug/keyboard_event_example
```

**Key Concepts**: Essential patterns for game and interactive application input.

#### `lambda_event_handler_test` - Lambda Event Handlers
**Purpose**: Shows how to use lambda functions as event handlers.

**Features**:
- ✅ Lambda-based event registration
- ✅ Capture by reference and value
- ✅ Type-safe lambda event handling
- ✅ Modern C++ event patterns

**Usage**:
```bash
./build/ninja-mc/bin/Debug/lambda_event_handler_test
```

**Key Concepts**: Functional programming approach to event handling.

#### `test_sdl_app_event` - SDL AppEvent Integration
**Purpose**: Tests SDL_AppEvent integration with EventRouter.

**Features**:
- ✅ SDL_AppEvent to EventRouter bridge
- ✅ Application lifecycle events
- ✅ Event system integration testing
- ✅ BaseApplication framework validation

**Usage**:
```bash
./build/ninja-mc/bin/Debug/test_sdl_app_event
```

**Key Concepts**: Internal event system integration testing.

### Application Framework Examples

#### `application_example` - BaseApplication Framework
**Purpose**: Demonstrates the BaseApplication framework usage.

**Features**:
- ✅ BaseApplication inheritance pattern
- ✅ Application lifecycle management
- ✅ SDL subsystem initialization
- ✅ Window and renderer setup

**Usage**:
```bash
./build/ninja-mc/bin/Debug/application_example
```

**Key Concepts**: Foundation pattern for SDL3 applications.

#### `message_box_demo` - MessageBox Builder Pattern
**Purpose**: Demonstrates the MessageBox builder pattern.

**Features**:
- ✅ Fluent MessageBox API
- ✅ Custom button configuration
- ✅ Different message types (Info, Warning, Error)
- ✅ Modal dialog handling

**Usage**:
```bash
./build/ninja-mc/bin/Debug/message_box_demo
```

**Key Concepts**: Modern C++ builder pattern for UI dialogs.

### Testing and Validation

#### `sdl_driver_test` - SDL Driver Verification
**Purpose**: Basic SDL functionality verification.

**Features**:
- ✅ SDL initialization testing
- ✅ Driver enumeration
- ✅ Basic functionality validation

**Usage**:
```bash
./build/ninja-mc/bin/Debug/sdl_driver_test
```

#### `visitor_pattern` - Visitor Pattern Implementation
**Purpose**: Advanced visitor pattern demonstration.

**Features**:
- ✅ Modern C++ visitor implementation
- ✅ Type-safe polymorphic dispatch
- ✅ Template-based visitor design

**Usage**:
```bash
./build/ninja-mc/bin/Debug/visitor_pattern
```

**Key Concepts**: Advanced design patterns for event systems.

## Shader Development Workflow

### Directory Structure
```
src/snippets/
├── shaders/                          # Shader source files
│   ├── triangle.vert                 # GLSL vertex shader source
│   ├── triangle.frag                 # GLSL fragment shader source
│   ├── triangle_vert.h               # Generated SPIRV C header
│   ├── triangle_frag.h               # Generated SPIRV C header
│   ├── compile_shaders.sh            # Compilation automation script
│   └── README.md                     # Detailed shader documentation
├── src/                              # C++ snippet sources
└── README.md                         # This documentation
```

### Shader Development Process

#### 1. Edit GLSL Source
```bash
# Edit the human-readable GLSL files
vim src/snippets/shaders/triangle.vert
vim src/snippets/shaders/triangle.frag
```

#### 2. Compile to SPIRV
```bash
cd src/snippets/shaders
./compile_shaders.sh
```

#### 3. Build and Test
```bash
cd /workspaces/sdl_demo
cmake --build --preset ninja-mc-debug --target shader_demo
./build/ninja-mc/bin/Debug/shader_demo
```

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

## Development Patterns

### Event System Architecture
The SDL3 C++ wrapper provides multiple event handling patterns:

1. **Traditional Inheritance**: `EventHandler<EventType>`
2. **Lambda Functions**: `registerEventHandler<EventType>(lambda)`
3. **Custom Events**: `CustomUserEvent<YourEventType>`
4. **Event Adaptation**: `EventAdaptor` for platform event conversion

### Application Framework
All examples follow the `BaseApplication` pattern:
- `init()`: Setup resources and configuration
- `iterate()`: Main loop iteration (automatic)
- Service registry for dependency management

### Resource Management
- RAII patterns throughout
- Smart pointers for automatic cleanup
- Exception-based error handling via `sdl::Exception`

## Learning Path

For new users, we recommend this progression:

1. **Start Simple**: `message_box_demo` → `application_example`
2. **Learn Events**: `keyboard_event_example` → `custom_events_example`
3. **Graphics Basics**: `font_hello_world` → `gpu_demo`
4. **Advanced Graphics**: `shader_demo`
5. **Advanced Patterns**: `event_adaptor_example` → `lambda_event_handler_test`

## Build Commands

Build specific targets:
```bash
# Build all snippets
cmake --build --preset ninja-mc-debug

# Build specific snippets
cmake --build --preset ninja-mc-debug --target font_hello_world
cmake --build --preset ninja-mc-debug --target shader_demo
cmake --build --preset ninja-mc-debug --target custom_events_example
```

## Previous Organization

This directory was previously organized with separate documentation files:
- `README_SHADER_DEMOS.md` (now integrated above)
- `SHADER_ORGANIZATION.md` (now integrated above)

Multiple experimental shader versions have been cleaned up, leaving only the working implementations. The remaining files represent battle-tested, production-ready examples of SDL3 C++ wrapper usage.