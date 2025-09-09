#ifndef SNAKE_UI_H
#define SNAKE_UI_H

#include <memory>

#include <sdl/window.h>
#include <sdl/renderer.h>
#include <sdl/color.h>

#include "constants.h"
#include "state.h"

namespace snake { 

class SnakeUI {
  public:
    void render([[maybe_unused]] const GameState& state);

  private:
    std::unique_ptr<sdl::Window> _window = std::make_unique<sdl::Window>("Snake", kInitialWindowWidth, kInitialWindowHeight);
    std::unique_ptr<sdl::Renderer> _renderer = std::make_unique<sdl::Renderer>(*_window);
};

}

#endif // SNAKE_UI_H
