#include <sdl/application.h>
#include <chrono>

#include <sdl/sdl.h>
#include <sdl/event_router.h>

#include "snake.h"
#include "snake_ui.h"

using namespace snake;

class SnakeApplication : public sdl::BaseApplication {
 public:
  auto init() -> bool override { 
    auto& sdl = requestSDL();
    sdl.initSubSystem(sdl::SDL::kVideo);
    auto eventRouter = getEventRouter();

    _snake = std::make_unique<Snake>(eventRouter);
    _snakeUI = std::make_unique<SnakeUI>();

    // auto layer = Layer<GameState>(
    //   [this]() {}
    // )

    _lastUpdateTime = std::chrono::steady_clock::now();

    return true; 
  };
  
  auto iterate() -> bool override { 
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _lastUpdateTime);
    
    if (elapsed >= std::chrono::milliseconds(250)) {
      _snake->update();
      _lastUpdateTime = currentTime;
    }
    
    _snakeUI->render(_snake->getState());
    return true; 
  };

  auto quit() -> void override {};

 private:
  std::unique_ptr<Snake>   _snake = nullptr;
  std::unique_ptr<SnakeUI> _snakeUI = nullptr;
  std::chrono::steady_clock::time_point _lastUpdateTime;

};

REGISTER_APPLICATION(SnakeApplication);

