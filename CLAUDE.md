# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Be brutally honest, don't be a yes man. If I am wrong, point it out bluntly. I need honest feedback on my code.

## Build System

This project uses CMake with presets for configuration and building. The following commands are for Ubuntu, on Windows `msvcc` should be used instead of `ninja-mc` in all the preset names.

- **Configure**: `cmake --preset ninja-mc` (uses Ninja Multi-Config generator)
- **Build Debug**: `cmake --build --preset ninja-mc-debug`
- **Build Release**: `cmake --build --preset ninja-mc-release`
- **Run Tests (in Debug mode)**: `ctest --preset ninja-mc-debug` (after Build Debug)
- **Run Tests (in Release mode)**: `ctest --preset ninja-mc-debug` (after Build Release)

## Performance Benchmarks

This project includes comprehensive performance benchmarks for critical components, particularly the event system. Benchmarks are built using Google Benchmark and are disabled by default to keep build times fast.

### Building and Running Benchmarks

The following commands are for Ubuntu, on Windows `msvcc` should be used instead of `ninja-mc` in all the preset names.

**Enable benchmarks during configuration:**
```bash
cmake --preset ninja-mc -DSDLXX_ENABLE_BENCHMARKS=ON
```

**Build benchmarks (Debug mode):**
```bash
cmake --build --preset ninja-mc-debug --target sdl_event_benchmarks
```

**Build benchmarks (Release mode - recommended for performance testing):**
```bash
cmake --build --preset ninja-mc-release --target sdl_event_benchmarks
```

**Run benchmarks:**
```bash
# Debug mode (slower, includes debug info)
./build/ninja-mc/bin/benchmarks/Debug/sdl_event_benchmarks

# Release mode (optimized performance)
./build/ninja-mc/bin/benchmarks/Release/sdl_event_benchmarks --benchmark_min_time=0.1s
```

### Event System Performance Characteristics

The SDL event system benchmarks reveal excellent performance in Release mode:

- **Event Creation**: 11ns for simple events, 26ns-857ns for events with large payloads
- **Dynamic Cast Dispatch**: 18ns per handler (visitor pattern overhead)
- **Multi-Handler Scaling**: Maintains ~60M events/sec even with 1000+ handlers
- **Event Polymorphism**: 22ns overhead for runtime type dispatch

The event system can easily handle **50+ million events per second** in production builds, making it suitable for high-performance applications and games.

### Benchmark Organization

Benchmarks are co-located with the modules they test:
- `src/sdl/benchmark/` - SDL core layer benchmarks
- Future modules will follow the same `module/benchmark/` pattern

## Architecture Overview

This is a C++20 SDL3 C++ wrapper project with a layered architecture:

### Core Layers
1. **sdl** - Low-level C++ wrapper around SDL3 C API
   - Provides RAII wrappers for SDL resources (Window, Renderer, Surface, Texture)
   - Event system with type-safe event handling
   - Exception-based error handling via `sdl::Exception`

2. **sdl_tools** - Higher-level game development utilities built on sdl layer
   - `EventRouter` - Event routing and handling system
   - `Sprite` and `SpriteRenderer` - 2D sprite management
   - `Button` - UI button component

### Sample Applications
- **examples/tictactoe** - Interactive tic-tac-toe game demonstrating SDL3 graphics, event handling, and UI components
- **snippets** - Demonstrations of individual features.
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
- **Tests of the public interfaces should be placed in the `test` folder; tests agains internal or SDL types should be placed in `impl_test`.** - use this for temporary diagnostic tools, debugging utilities, and experimental code
- **Tests must NEVER include `*_impl.h` headers** - tests should only interact with public interfaces

### Dependencies
- SDL3 and SDL3_image are fetched automatically via CMake FetchContent
- GoogleTest and Google Benchmark for testing and performance measurement
- All external dependencies are managed in `external/CMakeLists.txt`

### Code Style
- Raw pointers should never be used.
- Prefer trailing return types.
- **Header guards**: Use traditional `#ifndef`/`#define`/`#endif` include guards instead of `#pragma once`. Follow the pattern `#ifndef PROJECT_MODULE_FILENAME_H` / `#define PROJECT_MODULE_FILENAME_H` / `#endif // PROJECT_MODULE_FILENAME_H`
- **Unused variables**: Use `[[maybe_unused]]` attribute instead of void casting `(void)variable` to suppress unused variable warnings.
- SDL++ should contain as little functionality as possible, and instead focus on wrapping SDL3 functionality. (For example, don't static_cast<float> members of the Rectangle<int32_t> class to convert it to a Rectangle<float>, instead use SDL_RectToFRect )
- Where functionality is provided on top of SDL3 this should, were possible, be contained in the sdl_tools project.

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

## Additional Points

- Keep PROGRESS.md and README.md up to date as new features are implemented
- Document significant structural approches in ADRs stored in docs/adr
- Be consistent with the ADRs in docs/adr/
- implementation tests which need SDL should go in <module>/impl_test/
