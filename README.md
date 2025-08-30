# SDL++ - Modern C++20 Wrapper for SDL3

A comprehensive, type-safe C++20 wrapper around SDL3 that provides RAII resource management, modern C++ idioms, and a clean API for game development and multimedia applications.

## Features

- **🔒 Type Safety**: Compile-time type checking with comprehensive exception handling
- **⚡ Performance**: Zero-overhead abstractions with move-only semantics
- **🎯 Modern C++20**: Leverages latest language features including concepts, constexpr, and CTAD
- **🏗️ RAII**: Automatic resource management for all SDL objects
- **🎮 Game-Ready**: High-level tools for sprites, UI components, and event handling
- **📚 Well-Documented**: Comprehensive API documentation with practical examples
- **🧪 Thoroughly Tested**: Ensures reliability

## Architecture

SDL++ follows a clean layered architecture with optional modules that provides zero-overhead abstractions over SDL3.

📋 **[View Detailed Architecture Documentation](docs/pages/architecture.md)**

Key architectural features:
- **RAII Resource Management**: Automatic cleanup of all SDL resources
- **Move-Only Semantics**: Optimal performance with clear ownership
- **Type-Safe Events**: Compile-time event handling with std::variant
- **Service Registry**: Dependency injection for modular features

## Quick Start

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.20+
- SDL3 and SDL3_image (automatically fetched)

### Building

```bash
# Configure with Ninja Multi-Config
cmake --preset ninja-mc

# Build Debug version
cmake --build --preset ninja-mc-debug

# Build Release version  
cmake --build --preset ninja-mc-release

# Run tests
ctest --preset ninja-mc-debug

# Generate documentation (requires Doxygen)
cmake --build --preset ninja-mc-debug --target docs
```

### Basic Example

SDL++ provides an Application framework with automatic service management for clean, maintainable code:

```cpp
#include <application.h>
#include <window.h>
#include <renderer.h>
#include <texture.h>
#include <ttf_service.h>  // Optional: auto-registers TTF service when linked

using namespace sdl;

class MyGame : public BaseApplication {
 public:
  auto init() -> bool override {
    // Framework manages SDL lifecycle - no manual initialization
    auto& sdl = requestSDL();
    auto& ttf = requestService<TTF>();  // Optional services via registry
    
    // Initialize only the subsystems you need
    sdl.initSubSystem(SDL::kVideo | SDL::kEvents);
    
    // Create application resources
    window_ = std::make_unique<Window>("SDL++ Demo", 800, 600);
    renderer_ = std::make_unique<Renderer>(*window_);
    sprite_ = std::make_unique<Texture>(*renderer_, "player.png");
    
    return true;
  }

  auto iterate() -> bool override {
    // Render frame
    renderer_->setDrawColour(NamedColor::kBlack);
    renderer_->clear();
    renderer_->copy(*sprite_);
    renderer_->present();
    
    return true;  // Continue running
  }

  auto quit() -> void override {
    // Framework handles SDL cleanup automatically
  }

 private:
  std::unique_ptr<Window> window_;
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<Texture> sprite_;
};

// Register application with framework
REGISTER_APPLICATION(MyGame)
```

## Performance Benchmarks

SDL++ includes comprehensive performance benchmarks to validate the zero-overhead abstraction claims. The event system has been thoroughly benchmarked and shows exceptional performance characteristics.

### Running Benchmarks

```bash
# Enable benchmarks during configuration
cmake --preset ninja-mc -DSDLXX_ENABLE_BENCHMARKS=ON

# Build and run Release mode benchmarks (recommended)
cmake --build --preset ninja-mc-release --target sdl_event_benchmarks
./build/ninja-mc/bin/benchmarks/Release/sdl_event_benchmarks --benchmark_min_time=0.1s
```

### Performance Results

The SDL++ event system demonstrates excellent performance in Release builds:

| Operation | Performance | Notes |
|-----------|-------------|-------|
| **Event Creation** | 11ns | Simple events with minimal overhead |
| **Event Dispatch** | 18ns per handler | Dynamic cast visitor pattern |
| **Multi-Handler Scaling** | 60M+ events/sec | Linear scaling up to 1000+ handlers |
| **Event Polymorphism** | 22ns overhead | Runtime type dispatch |

The event system easily handles **50+ million events per second**, making it suitable for high-performance games and real-time applications.

## Examples and Advanced Features

🎮 **[View Complete Examples and Tutorials](docs/pages/examples.md)**

The repository includes comprehensive examples and advanced usage patterns:
- **TicTacToe Game**: Complete interactive game with UI, events, and graphics
- **Pong Game**: Real-time collision detection, keyboard input, and primitive drawing
- **Event System**: Lambda-based event handling with type safety
- **Sprite Rendering**: Efficient batch rendering with texture sharing
- **UI Components**: Interactive buttons and form elements
- **TTF Integration**: Text rendering with service management

## Documentation

### Generating Documentation

SDL++ uses Doxygen for comprehensive API documentation:

```bash
# Generate HTML documentation (requires Doxygen)
cmake --build --preset ninja-mc-debug --target docs

# Open documentation in browser
open docs/output/html/index.html  # macOS
xdg-open docs/output/html/index.html  # Linux
```

The generated documentation includes:
- **API Documentation**: Complete reference for all public APIs
- **Architecture Overview**: System design with integrated ADRs
- **Examples**: Code samples and usage patterns

### Other Documentation

- **Architecture Decision Records**: Design rationale and trade-offs in `docs/adr/`
- **Build Instructions**: Detailed build configuration in `CLAUDE.md`

## Key Features

- **Zero-overhead abstractions**: No runtime cost over raw SDL
- **RAII resource management**: Automatic cleanup eliminates resource leaks  
- **Type-safe event handling**: Compile-time event dispatch with std::variant
- **Collision detection**: Built-in Rectangle intersection and containment methods
- **Primitive drawing**: Line and shape rendering with hardware acceleration
- **Exception safety**: All SDL errors converted to typed exceptions
- **Modern C++20**: Concepts, constexpr, and CTAD throughout

## Contributing

1. Follow the existing code style (trailing return types, RAII patterns)
2. Add tests for new functionality
3. Update documentation for public APIs  
4. Use traditional `#ifndef` include guards
5. Place test/debug code in the `snippets` project

## Requirements

- **Compiler**: C++20 support required
- **CMake**: 3.20 or later
- **SDL3**: Automatically fetched via CMake
- **GoogleTest**: For running tests (automatically fetched)

## License

[Add your license information here]

## Acknowledgments

Built on the excellent SDL3 library by Sam Lantinga and the SDL development team.
