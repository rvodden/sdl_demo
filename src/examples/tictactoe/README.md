# Tic-Tac-Toe SDL++ Demo

An interactive tic-tac-toe game demonstrating key features of the SDL++ wrapper library. This example showcases modern C++ patterns, event-driven architecture, and SDL3 integration.

![Initial Game State](screenshots/initial_state.png)
*The game board with empty cells ready for player interaction*

## Featured SDL++ Components

### Core SDL Layer Features

#### **RAII Resource Management**
- **Window**: Automatic window creation and cleanup (`src/tictactoe_ui.cpp:29`)
- **Renderer**: Safe renderer lifecycle management with shared ownership
- **Texture**: Automatic texture loading from embedded binary data using `_binary_tic_tac_toe_png_start`
- **Surface**: Memory-safe surface handling

#### **Event System Architecture**
The game demonstrates SDL++'s type-safe, visitor pattern-based event system:

- **Custom User Events**: Type-safe custom events extending `CustomUserEvent`
  - `ClickEvent`: Cell click coordinates with timestamp
  - `GameCompletedEvent`: Game end state with win condition
  - `StartNewGameEvent`: Game reset trigger
  - `TurnEvent<Player>`: Templated turn notifications

- **Event Bus Pattern**: Decoupled event publishing and handling (`src/main.cpp:29`)
- **Event Router**: Type-safe event dispatch with lambda handlers (`src/main.cpp:66-113`)

#### **Exception-Based Error Handling**
- SDL errors automatically converted to C++ exceptions
- Clean error propagation through `sdl::Exception`

### SDL Tools Layer Features

#### **Sprite System**
- **Sprite**: Texture region management for game assets (`src/tictactoe_ui.cpp:39-45`)
- **SpriteRenderer**: Hardware-accelerated sprite rendering (`src/tictactoe_ui.cpp:49-53`)
- **Texture Blending**: Alpha blending support (`src/tictactoe_ui.cpp:12`)

#### **UI Components**
- **Button**: Interactive button components with event handling (`src/tictactoe_ui.cpp:19-33`)
- **Event Binding**: Mouse event registration with coordinate capture
- **Hit Testing**: Automatic coordinate-to-cell mapping

#### **Application Framework**
- **BaseApplication**: Structured game loop with init/iterate/quit lifecycle
- **ApplicationRunner**: Singleton application management with event integration
- **Message Box**: Native dialog integration for game results (`src/main.cpp:104`)

### Modern C++ Features

#### **Smart Pointer Management**
- `std::shared_ptr` for resource sharing across components
- `std::unique_ptr` for exclusive ownership of events and buttons
- Automatic memory management prevents resource leaks

#### **Template Metaprogramming**
- Template-based turn events: `TurnEvent<Player::kX>`, `TurnEvent<Player::kO>`
- Type-safe event dispatch without runtime type checks

#### **Range-Based Iteration**
Modern C++20 ranges for grid iteration (`src/tictactoe_ui.cpp:14-16`):
```cpp
for (uint8_t x : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellColumns}) {
    for (uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellRows}) {
        // Cell processing logic
    }
}
```

#### **Lambda Event Handlers**
Concise event handling with capture semantics (`src/main.cpp:66-69`):
```cpp
eventRouter->registerEventHandler<ClickEvent>(
    [this](const ClickEvent& clickEvent) -> void {
        _ticTacToe->play(clickEvent.x, clickEvent.y);
    });
```

## Game Screenshots

### Initial State
![Game Start](screenshots/game_start.png)
*Clean 3x3 grid ready for the first move*

### Mid-Game
![Mid Game](screenshots/mid_game.png)
*Players alternating moves with X's and O's visible on the board*

### Game Complete
![Win State](screenshots/win_state.png)
*Victory dialog showing the winner with option to start a new game*

## Architecture Highlights

### Event-Driven Design
The game uses a clean separation between game logic (`TicTacToe`) and presentation (`TicTacToeUI`):

1. **UI Layer**: Handles SDL events and converts to game events
2. **Game Logic**: Processes game events and publishes state changes
3. **Event Bus**: Coordinates communication between layers

### Asset Management
- **Embedded Resources**: Game sprites embedded directly in binary
- **Sprite Atlas**: Single texture with multiple sprite regions
- **Coordinate Mapping**: Automatic translation from screen to grid coordinates

### Performance Considerations
- **Hardware Acceleration**: GPU-accelerated rendering through SDL3
- **Efficient Redraw**: Only renders when game state changes
- **Memory Efficiency**: Shared resources and minimal allocations

## Building and Running

```bash
# Configure build system
cmake --preset ninja-mc

# Build the game
cmake --build --preset ninja-mc-debug --target tictactoe

# Run the game
./build/ninja-mc/bin/Debug/tictactoe
```

## Code Organization

```
src/examples/tictactoe/
├── include/
│   ├── tictactoe.h              # Game logic interface
│   ├── tictactoe_ui.h           # UI layer interface
│   ├── events.h                 # Custom event definitions
│   └── game_constants.h         # Game configuration
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── tictactoe.cpp            # Game logic implementation
│   └── tictactoe_ui.cpp         # SDL++ rendering and input
└── data/
    └── tic_tac_toe.png          # Game sprite atlas
```

This example demonstrates SDL++'s goal of providing modern C++ abstractions over SDL3 while maintaining performance and enabling clean, maintainable code architecture.