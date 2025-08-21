# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses CMake with presets for configuration and building:

- **Configure**: `cmake --preset ninja-mc` (uses Ninja Multi-Config generator)
- **Build Debug**: `cmake --build --preset ninja-mc-debug-all`
- **Build Release**: `cmake --build --preset ninja-mc-release-all`
- **Run Tests**: `ctest --preset ninja-mc-debug-all` (after building debug)

## Architecture Overview

This is a C++20 SDL3 C++ wrapper project with a layered architecture:

### Core Layers
1. **sdl** - Low-level C++ wrapper around SDL3 C API
   - Provides RAII wrappers for SDL resources (Window, Renderer, Surface, Texture)
   - Event system with type-safe event handling
   - Exception-based error handling via `sdl::Exception`

2. **sdl_tools** - Higher-level game development utilities built on sdl layer
   - `EventDispatcher` - Event routing and handling system
   - `Sprite` and `SpriteRenderer` - 2D sprite management
   - `Button` - UI button component

3. **utils** - General-purpose C++ utilities
   - `constexpr_map` - Compile-time key-value mapping with automatic size deduction
   - `thread_safe_queue` - Thread-safe queue implementation

### Sample Applications
- **examples/tictactoe** - Interactive tic-tac-toe game demonstrating SDL3 graphics, event handling, and UI components
- **snippets** - C++ pattern demonstrations and debugging utilities (visitor pattern, polymorphic collections, SDL diagnostics)
- **visitor_pattern_mockup** - Advanced visitor pattern implementation

## Development Conventions

### Build System Patterns
- Each module has its own `CMakeLists.txt`
- Tests are automatically discovered via GoogleTest integration
- Public Headers go in `include/` directories, sources in `src/`, private headers in `src/include`

### Code Organization
- Implementation details are hidden in `*_impl.h` files (pimpl pattern)
- Public headers expose clean interfaces without SDL dependencies
- Namespaces follow directory structure (`sdl::`, `sdl::tools::`)
- **Test/Debug code should be placed in the `snippets` project** - use this for temporary diagnostic tools, debugging utilities, and experimental code
- **Tests must NEVER include `*_impl.h` headers** - tests should only interact with public interfaces

### Dependencies
- SDL3 and SDL3_image are fetched automatically via CMake FetchContent
- GoogleTest and Google Benchmark for testing and performance measurement
- All external dependencies are managed in `external/CMakeLists.txt`

### Code Style
- Raw pointers should never be used.
- Prefer trailing return types.

## Key Features

### Constexpr Map with Automatic Size Deduction
The `utils/constexpr_map.h` provides compile-time key-value mapping with C++17 CTAD support:

```cpp
// Old way (manual size specification):
static constexpr vodden::Map<Key, Value, 5> myMap {{...}};

// New way (automatic size deduction):
static constexpr auto dataArray = std::array{
    std::pair{Key1, Value1},
    std::pair{Key2, Value2}
};
static constexpr auto myMap = vodden::Map(dataArray);
```

### Modern C++ Patterns
- Uses `std::shared_ptr` for resource sharing (e.g., in TicTacToeUI)
- RAII for SDL resource management
- Exception-based error handling
- Type-safe event system with custom user events

## Testing

Tests are located in `test/` subdirectories and use GoogleTest framework. The build system automatically creates test executables and registers them with CTest.
