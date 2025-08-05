# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses CMake with presets for configuration and building:

- **Configure**: `cmake --preset ninja-mc` (uses Ninja Multi-Config generator)
- **Build Debug**: `cmake --build --preset ninja-mc-debug-all`
- **Build Release**: `cmake --build --preset ninja-mc-release-all`
- **Run Tests**: `ctest --preset ninja-mc-debug-all` (after building debug)

## Architecture Overview

This is a C++20 SDL2 demonstration project with a layered architecture:

### Core Layers
1. **sdl** - Low-level C++ wrapper around SDL2 C API
   - Provides RAII wrappers for SDL resources (Window, Renderer, Surface, Texture)
   - Event system with type-safe event handling
   - Exception-based error handling via `sdl::Exception`

2. **sdl_tools** - Higher-level game development utilities built on sdl layer
   - `EventDispatcher` - Event routing and handling system
   - `Sprite` and `SpriteRenderer` - 2D sprite management
   - `Button` - UI button component

3. **utils** - General-purpose C++ utilities
   - `value_ptr` - Polymorphic value semantics wrapper
   - `constexpr_map` - Compile-time key-value mapping
   - `thread_safe_queue` - Thread-safe queue implementation

### Sample Applications
- **sdl_c** - Basic C-style SDL usage
- **sdl_oop** - Object-oriented SDL demonstration
- **snippets** - C++ pattern demonstrations (visitor pattern, polymorphic collections)
- **visitor_pattern_mockup** - Advanced visitor pattern implementation

## Development Conventions

### Build System Patterns
- Each module has its own `CMakeLists.txt` using `standard_build()` macro
- Shared libraries are built by default with `standard_libarary_build()`
- Executables use `standard_executable_build()`
- Tests are automatically discovered via GoogleTest integration
- Headers go in `include/` directories, sources in `src/`

### Code Organization
- Implementation details are hidden in `*_impl.h` files (pimpl pattern)
- Public headers expose clean interfaces without SDL dependencies
- Namespaces follow directory structure (`sdl::`, `sdl::tools::`)

### Dependencies
- SDL2 and SDL2_image are fetched automatically via CMake FetchContent
- GoogleTest and Google Benchmark for testing and performance measurement
- All external dependencies are managed in `external/CMakeLists.txt`

## Testing

Tests are located in `test/` subdirectories and use GoogleTest framework. The build system automatically creates test executables and registers them with CTest.