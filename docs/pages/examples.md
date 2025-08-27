# Examples and Tutorials {#examples}

This page contains practical examples demonstrating SDL++ features and usage patterns.

## TicTacToe Game Example

The TicTacToe example (`src/examples/tictactoe/`) demonstrates a complete interactive game using SDL++ components:

### Key Features Demonstrated

- **Application Framework Integration**: Uses `BaseApplication` for lifecycle management
- **Event Handling**: Mouse click detection with `EventRouter` and `Button` components  
- **2D Rendering**: Texture loading, sprite rendering, and drawing operations
- **UI Components**: Interactive buttons with hover states and click handlers
- **Service Management**: TTF service integration for text rendering
- **Resource Management**: RAII-based texture and surface management

### Code Highlights

#### Application Structure

```cpp
#include <application.h>
#include <tictactoe_ui.h>

class TicTacToeApp : public sdl::BaseApplication {
public:
    auto init() -> bool override {
        // Framework handles SDL initialization
        auto& sdl = requestSDL();
        auto& ttf = requestService<sdl::TTF>();
        
        sdl.initSubSystem(sdl::SDL::kVideo | sdl::SDL::kEvents);
        
        ui_ = std::make_unique<TicTacToeUI>(800, 600);
        return ui_->init();
    }
    
    auto iterate() -> bool override {
        return ui_->handleEvents() && ui_->render();
    }
    
private:
    std::unique_ptr<TicTacToeUI> ui_;
};

REGISTER_APPLICATION(TicTacToeApp)
```

#### Interactive UI with Event Handling

```cpp
// Create interactive game board cells
for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
        auto cellBounds = FloatRectangle(
            boardX + col * cellSize, 
            boardY + row * cellSize,
            cellSize, cellSize
        );
        
        auto button = std::make_unique<Button>(eventRouter_, cellBounds);
        
        // Lambda handler for cell clicks
        button->registerEventHandler([this, row, col](const MouseButtonEvent& e) {
            if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
                handleCellClick(row, col);
            }
        });
        
        boardButtons_[row][col] = std::move(button);
    }
}
```

#### Resource Management with RAII

```cpp
// Automatic resource cleanup via RAII
class TicTacToeUI {
private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<Texture> backgroundTexture_;
    std::unique_ptr<Texture> xTexture_;
    std::unique_ptr<Texture> oTexture_;
    
    // Resources automatically cleaned up in destructor
};
```

## Sprite System Example

Demonstrates efficient sprite rendering with texture sharing:

```cpp
// Load sprite sheet
sdl::Texture spriteSheet(renderer, "sprites.png");
auto sharedTexture = std::make_shared<const sdl::Texture>(std::move(spriteSheet));

// Create sprites from different regions  
sdl::tools::Sprite player(sharedTexture, FloatRectangle(0, 0, 32, 48));
sdl::tools::Sprite enemy(sharedTexture, FloatRectangle(32, 0, 32, 48));

// Efficient batch rendering
sdl::tools::SpriteRenderer spriteRenderer(renderer);
spriteRenderer.addSprite(player, 100, 200);
spriteRenderer.addSprite(enemy, 200, 200);  
spriteRenderer.render(); // Single draw call for both sprites
```

## Event System with Lambda Handlers

Type-safe event handling with modern C++ features:

```cpp
auto eventRouter = std::make_shared<sdl::tools::EventRouter>(eventBus);

// Register lambda handlers for different event types
eventRouter->registerEventHandler<MouseButtonEvent>([](const MouseButtonEvent& e) {
    if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
        std::cout << "Left click at " << e.x << ", " << e.y << std::endl;
    }
});

eventRouter->registerEventHandler<KeyboardEvent>([](const KeyboardEvent& e) {
    if (e.down && e.key == SDLK_SPACE) {
        std::cout << "Space key pressed" << std::endl;
    }
});

// Event loop with automatic dispatching
eventRouter->run();
```

## Surface Operations Example

Pixel-level operations with bounds checking:

```cpp
// Create surface and manipulate pixels
sdl::Surface surface(320, 240);

// Fill with color
surface.fill(sdl::NamedColor::kBlue);

// Set individual pixels
surface.setPixel(160, 120, sdl::NamedColor::kRed);

// Get pixel colors  
auto centerPixel = surface.getPixel(160, 120);

// Blit operations
sdl::Surface source("sprite.bmp");
surface.blit(source, sourceRect, destRect);

// Convert to texture for rendering
sdl::Texture texture(renderer, surface);
```

## Constexpr Utilities Example

Compile-time data structures with automatic size deduction:

```cpp
// Modern C++17 CTAD approach
static constexpr auto colorMapping = std::array{
    std::pair{PlayerType::kX, sdl::NamedColor::kRed},
    std::pair{PlayerType::kO, sdl::NamedColor::kBlue}
};

static constexpr auto playerColors = vodden::Map(colorMapping);

// Compile-time lookup
constexpr auto xColor = playerColors.at(PlayerType::kX);
```

## Message Box Example

Native dialog integration with builder pattern:

```cpp
// Simple message box
sdl::MessageBox("Game Over", "Thanks for playing!")
    .setType(sdl::MessageBox::Type::kInformation)
    .show();

// Complex dialog with custom buttons
auto result = sdl::MessageBox("Save Game", "Do you want to save before quitting?")
    .setType(sdl::MessageBox::Type::kQuestion)
    .addButton("Save", sdl::MessageBox::ButtonFlag::kReturnKeyDefault)
    .addButton("Don't Save")
    .addButton("Cancel", sdl::MessageBox::ButtonFlag::kEscapeKeyDefault)
    .show();

switch (result.buttonIndex) {
    case 0: saveGame(); break;
    case 1: /* don't save */ break;
    case 2: return; // cancel
}
```

## Error Handling

Exception-based error handling eliminates manual error checking:

```cpp
try {
    // SDL++ operations that might fail
    auto window = sdl::Window("My Game", 1920, 1080);
    auto renderer = sdl::Renderer(window);
    auto texture = sdl::Texture(renderer, "missing_file.png");
    
} catch (const sdl::Exception& e) {
    std::cerr << "SDL++ Error: " << e.what() << std::endl;
    // Automatic resource cleanup via RAII
    return false;
}
```

## Next Steps

1. **Study the TicTacToe Implementation**: Complete working example in `src/examples/tictactoe/`
2. **Explore Code Snippets**: Additional examples in `src/snippets/src/`
3. **Check the Test Suite**: Unit tests demonstrate API usage patterns in `src/*/test/`
4. **Review Architecture Decisions**: Design rationale in `docs/adr/`

## Building and Running Examples

```bash
# Build all examples
cmake --build --preset ninja-mc-debug

# Run TicTacToe
./build/ninja-mc/bin/Debug/tictactoe

# Run code snippets  
./build/ninja-mc/bin/Debug/message_box_demo
./build/ninja-mc/bin/Debug/font_hello_world
```