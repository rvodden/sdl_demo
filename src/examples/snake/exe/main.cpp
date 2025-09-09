#include <sdl/application.h>

#include "snake.h"
#include "snake_ui.h"

using namespace snake;

class SnakeApplication : public sdl::BaseApplication {
 public:
  auto init() -> bool override { 
    _snake = std::make_unique<Snake>();
    _snakeUI = std::make_unique<SnakeUI>();

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
};

REGISTER_APPLICATION(SnakeApplication);

