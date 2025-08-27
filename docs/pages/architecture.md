# Architecture Overview {#architecture}

SDL++ follows a clean layered architecture with optional modules, providing a modern C++20 interface to SDL3 functionality.

## Layered Design

```
┌─────────────────────────────────────────────────────┐
│                Applications                         │
│  (TicTacToe, Snippets, Examples)                    │
├─────────────────────────────────────────────────────┤
│            Application Framework                    │
│  (Service Registry, Lifecycle Management)           │
├─────────────────────────────────────────────────────┤
│        Extensions          │      SDL++ Tools       │
│  (TTF Text Rendering)      │ (EventRouter, Sprites)  │
├─────────────────────────────────────────────────────┤
│                Core SDL++ Wrapper                   │
│  (Window, Renderer, Texture, Surface, Events)       │
├─────────────────────────────────────────────────────┤
│                  Utilities                          │
│        (Constexpr Map, Thread-Safe Queue)           │
└─────────────────────────────────────────────────────┘
```

## Core Design Principles

### RAII Resource Management

All SDL resources are automatically managed through RAII, eliminating resource leaks:

```cpp
{
    sdl::Window window("Title", 800, 600);
    sdl::Renderer renderer(window);
    // Resources automatically cleaned up on scope exit
}
```

**Benefits:**
- No manual cleanup required
- Exception-safe resource management  
- Impossible to forget cleanup calls
- Automatic ordering of destruction

### Move-Only Semantics

Expensive resources use move-only semantics for optimal performance:

```cpp
sdl::Texture texture = loadTexture("sprite.png");
sdl::Texture moved = std::move(texture); // Efficient transfer
// texture is now empty, moved owns the resource
```

**Benefits:**
- Zero-cost resource transfers
- Clear ownership semantics
- Prevents accidental copying of expensive resources
- Compile-time enforcement

### Exception-Based Error Handling

All SDL error conditions are converted to typed exceptions:

```cpp
try {
    auto window = sdl::Window("Game", 1920, 1080);
    auto renderer = sdl::Renderer(window);
} catch (const sdl::Exception& e) {
    std::cerr << "SDL Error: " << e.what() << std::endl;
}
```

**Benefits:**
- No manual error checking required
- Automatic error propagation
- Type-safe error handling
- Exception safety guarantees

### Type-Safe Event System

Events are type-safe variants, eliminating runtime casting errors:

```cpp
void handleEvent(const sdl::Event& event) {
    std::visit([](const auto& e) {
        if constexpr (std::is_same_v<std::decay_t<decltype(e)>, MouseButtonEvent>) {
            handleMouseClick(e);
        }
    }, event);
}
```

**Benefits:**
- Compile-time type checking
- No runtime casting failures
- Exhaustive event handling
- Modern C++ visitor pattern

## Module Descriptions

### Core Layer (`sdl`)

The foundation layer providing direct C++ wrappers around SDL3 functionality:

**Components:**
- **Window Management**: Hardware-accelerated window creation with VSync support
- **Rendering**: 2D hardware-accelerated rendering with blend modes and primitives  
- **Resource Management**: RAII wrappers for Textures, Surfaces, and SDL objects
- **Event System**: Type-safe event handling with visitor pattern support

**Design Goals:**
- Zero-overhead abstractions over SDL3
- Type safety and RAII
- Exception-based error handling
- Modern C++20 features

### Application Framework (`sdl_application`)

High-level application lifecycle management with dependency injection:

**Features:**
- **Service Registry**: Dependency injection for optional services (TTF, future audio)
- **Lifecycle Management**: Automatic initialization, main loop, and cleanup
- **Cross-Platform Entry**: Unified application entry point with `REGISTER_APPLICATION`

**Usage Pattern:**
```cpp
class MyApp : public BaseApplication {
    auto init() -> bool override;
    auto iterate() -> bool override;
    auto quit() -> void override;
};

REGISTER_APPLICATION(MyApp)
```

### Tools Layer (`sdl_tools`)

Game development utilities built on the core SDL layer:

**Components:**
- **EventRouter**: Centralized event distribution system with lambda support
- **Sprite System**: Efficient sprite sheet management with batch rendering  
- **UI Components**: Interactive buttons with automatic event handling

**Key Features:**
- High-performance batch rendering
- Modern C++ lambda event handlers
- Automatic resource sharing
- Composable UI components

### Extensions

Optional modules providing specialized functionality:

#### TTF Text Rendering (`sdl_ttf`)
- Font loading and management
- Text rendering with various quality modes
- Service integration for automatic lifecycle management
- RAII font resource management

#### Future Extensions
- Audio system integration
- Networking utilities  
- Physics integration helpers
- Platform-specific features

### Utilities (`utils`)

General-purpose C++ utilities supporting the framework:

**Components:**
- **Constexpr Map**: Compile-time key-value mapping with automatic size deduction
- **Thread-Safe Queue**: High-performance concurrent data structures

**Modern C++ Features:**
- C++17 CTAD (Class Template Argument Deduction)
- Constexpr evaluation
- Zero-overhead abstractions
- Type safety

## Implementation Patterns

### Pimpl Pattern

Implementation details are hidden using the pimpl (Pointer to Implementation) pattern:

```cpp
// Public header (window.h)
class Window {
public:
    Window(std::string title, uint16_t width, uint16_t height);
    ~Window();
    // ... public interface
private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

// Implementation (window_impl.h)
class Window::Impl {
    SDL_Window* window_;
    // SDL-specific implementation details
};
```

**Benefits:**
- Clean public API without SDL dependencies
- Faster compilation (no SDL headers in public interface)
- ABI stability
- Implementation flexibility

### Service Registry Pattern  

Optional services are managed through a dependency injection system:

```cpp
class MyApp : public BaseApplication {
    auto init() -> bool override {
        auto& sdl = requestSDL();           // Core service
        auto& ttf = requestService<TTF>();  // Optional service
        
        // Services automatically available and cleaned up
    }
};
```

**Benefits:**
- Automatic service lifecycle management
- Clear dependency declarations
- Optional feature integration
- Testability through service mocking

### Resource Sharing

Expensive resources like textures can be efficiently shared:

```cpp
// Create shared texture
auto sharedTexture = std::make_shared<const sdl::Texture>(
    sdl::Texture(renderer, "spritesheet.png")
);

// Multiple sprites can reference the same texture
sdl::tools::Sprite player(sharedTexture, playerRect);
sdl::tools::Sprite enemy(sharedTexture, enemyRect);

// Efficient batch rendering
spriteRenderer.addSprite(player, x1, y1);
spriteRenderer.addSprite(enemy, x2, y2);
spriteRenderer.render(); // Single draw call
```

**Benefits:**
- Memory efficiency through sharing
- Batch rendering optimization
- Automatic reference counting
- Cache-friendly data access

## Architecture Decision Records

Detailed design decisions and trade-offs are documented in Architecture Decision Records (ADRs):

- \subpage adr_0001 "ADR-0001: Layered Architecture" - Overall system structure
- \subpage adr_0002 "ADR-0002: Pimpl Pattern Usage" - Implementation hiding strategy  
- \subpage adr_0003 "ADR-0003: Event System Design" - Type-safe event handling
- \subpage adr_0004 "ADR-0004: RAII Resource Management" - Automatic cleanup
- \subpage adr_0005 "ADR-0005: C++20 Concepts for Template Constraints" - Type safety
- \subpage adr_0006 "ADR-0006: Application Framework Service Management" - Dependency injection

These ADRs provide detailed rationale for architectural choices and can guide future development decisions.