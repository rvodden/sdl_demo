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

SDL++ follows a clean three-layer architecture:

```
┌─────────────────────────────────────────────────────┐
│                Applications                         │
│  (TicTacToe, Snippets, Examples)                    │
├─────────────────────────────────────────────────────┤
│                SDL++ Tools                          │
│  (EventRouter, Sprite System, UI Components)        │
├─────────────────────────────────────────────────────┤
│                Core SDL++ Wrapper                   │
│  (Window, Renderer, Texture, Surface, Events)       │
└─────────────────────────────────────────────────────┘
```

### Core Layer (`sdlpp`)
- **Window Management**: Hardware-accelerated window creation with VSync support
- **Rendering**: 2D hardware-accelerated rendering with blend modes and primitives
- **Resource Management**: RAII wrappers for Textures, Surfaces, and SDL objects
- **Event System**: Type-safe event handling with visitor pattern support

### Tools Layer (`sdlpp_tools`)  
- **EventRouter**: Centralized event distribution system with lambda support
- **Sprite System**: Efficient sprite sheet management with batch rendering
- **UI Components**: Interactive buttons with automatic event handling

### Utilities (`utils`)
- **Constexpr Map**: Compile-time key-value mapping with automatic size deduction
- **Thread-Safe Queue**: High-performance concurrent data structures

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
```

### Basic Example

```cpp
#include <sdlpp/sdlpp.h>

int main() {
    // Initialize SDL
    sdlpp::SDL sdl;
    
    // Create window and renderer
    sdlpp::Window window("SDL++ Demo", 800, 600);
    sdlpp::Renderer renderer(window);
    
    // Load and display texture
    sdlpp::Texture sprite(renderer, "player.png");
    
    // Game loop
    bool running = true;
    while (running) {
        // Handle events
        sdlpp::Event event;
        while (sdlpp::pollEvent(event)) {
            if (auto* quit = std::get_if<sdlpp::QuitEvent>(&event)) {
                running = false;
            }
        }
        
        // Render
        renderer.setDrawColor(sdlpp::NamedColor::kBlack);
        renderer.clear();
        renderer.copy(sprite);
        renderer.present();
    }
    
    return 0;
}
```

## Advanced Features

### Event System with Lambda Handlers

```cpp
auto eventBus = std::make_shared<sdlpp::tools::EventBus>();
auto eventRouter = std::make_shared<sdlpp::tools::EventRouter>(eventBus);

// Register lambda handlers
eventRouter->registerEventHandler<MouseButtonEvent>([](const MouseButtonEvent& e) {
    if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
        std::cout << "Left click at " << e.x << ", " << e.y << std::endl;
    }
});

// Run event loop
eventRouter->run();
```

### Sprite System with Batch Rendering

```cpp
// Load sprite sheet
sdlpp::Texture spriteSheet(renderer, "characters.png");
auto sharedTexture = std::make_shared<const sdlpp::Texture>(std::move(spriteSheet));

// Create sprites from different regions
sdlpp::tools::Sprite player(sharedTexture, FloatRectangle(0, 0, 32, 48));
sdlpp::tools::Sprite enemy(sharedTexture, FloatRectangle(32, 0, 32, 48));

// Efficient batch rendering
sdlpp::tools::SpriteRenderer spriteRenderer(renderer);
spriteRenderer.addSprite(player, 100, 200);
spriteRenderer.addSprite(enemy, 200, 200);
spriteRenderer.render(); // Single draw call for both sprites
```

### Interactive UI Components

```cpp
// Create interactive button
FloatRectangle buttonBounds(100, 50, 200, 75);
sdlpp::tools::Button playButton(eventRouter, buttonBounds);

playButton.registerEventHandler([](const MouseButtonEvent& e) {
    if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
        std::cout << "Play button clicked!" << std::endl;
        startGame();
    }
});
```

### Constexpr Utilities with CTAD

```cpp
// Compile-time mapping with automatic size deduction
static constexpr auto colorMap = vodden::Map(std::array{
    std::pair{Key::kRed, sdlpp::NamedColor::kRed},
    std::pair{Key::kBlue, sdlpp::NamedColor::kBlue},
    std::pair{Key::kGreen, sdlpp::NamedColor::kGreen}
});

auto color = colorMap.at(Key::kRed); // Compile-time lookup
```

## Examples

The repository includes several comprehensive examples:

- **TicTacToe**: Complete interactive game demonstrating UI, events, and graphics
- **Snippets**: Pattern demonstrations and debugging utilities  
- **Visitor Pattern**: Advanced type-safe event handling examples

## Documentation

- **API Documentation**: Comprehensive Doxygen documentation for all public APIs
- **Architecture Decision Records**: Design rationale and trade-offs in `docs/adr/`
- **Build Instructions**: Detailed build configuration in `CLAUDE.md`

## Design Principles

### RAII Resource Management
All SDL resources are automatically managed through RAII, eliminating resource leaks:

```cpp
{
    sdlpp::Window window("Title", 800, 600);
    sdlpp::Renderer renderer(window);
    // Resources automatically cleaned up on scope exit
}
```

### Move-Only Semantics
Expensive resources use move-only semantics for optimal performance:

```cpp
sdlpp::Texture texture = loadTexture("sprite.png");
sdlpp::Texture moved = std::move(texture); // Efficient transfer
// texture is now empty, moved owns the resource
```

### Type-Safe Event Handling
Events are type-safe variants, eliminating runtime casting errors:

```cpp
void handleEvent(const sdlpp::Event& event) {
    std::visit([](const auto& e) {
        if constexpr (std::is_same_v<std::decay_t<decltype(e)>, MouseButtonEvent>) {
            handleMouseClick(e);
        }
    }, event);
}
```

## Performance Characteristics

- **Zero-overhead abstractions**: No runtime cost over raw SDL
- **Batch rendering**: Sprites sharing textures rendered in single draw calls
- **Memory efficient**: Shared texture references eliminate duplication
- **Compile-time optimizations**: Extensive use of `constexpr` and `[[nodiscard]]`

## Security

- **No raw pointers**: All memory managed through smart pointers
- **Bounds checking**: Surface operations validate coordinates
- **Exception safety**: Strong exception safety guarantees throughout
- **No undefined behavior**: All SDL error conditions converted to exceptions

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
