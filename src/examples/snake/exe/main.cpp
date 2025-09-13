#include <sdl/application.h>

#include <sdl/sdl.h>
#include <sdl/ticker.h>
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
    _ticker = std::make_unique<sdl::tools::Ticker>(getEventBus(), std::chrono::milliseconds(250));

    eventRouter->registerEventHandler<sdl::tools::TickEvent>([this](const sdl::tools::TickEvent&) {
      _snake->update();
    });

    _ticker->start();

    return true; 
  };
  
  auto iterate() -> bool override { 
    _snakeUI->render(_snake->getState());
    return true; 
  };

  auto quit() -> void override {};

 private:
  std::unique_ptr<Snake>   _snake = nullptr;
  std::unique_ptr<SnakeUI> _snakeUI = nullptr;
  std::unique_ptr<sdl::tools::Ticker> _ticker = nullptr;

};

REGISTER_APPLICATION(SnakeApplication);

