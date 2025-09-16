#include <sdl/random.h>

#include "snake.h"
#include "state.h"

namespace snake {

Snake::Snake(std::shared_ptr<sdl::tools::EventRouter> eventRouter) : _eventRouter(std::move(eventRouter)), _food(_placeFood()) {

    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kW, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      if(_direction == Direction::kDown) { return; }
      _nextDirection = Direction::kUp;
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kA, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      if(_direction == Direction::kRight) { return; }
      _nextDirection = Direction::kLeft;
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kS, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      if(_direction == Direction::kUp) { return; }
      _nextDirection = Direction::kDown;
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kD, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      if(_direction == Direction::kLeft) { return; }
      _nextDirection = Direction::kRight;
    });

};

void Snake::update() {
  _direction = _nextDirection;
  auto nextHead = kDirectionMap[_direction](_snakeBody->head());

  // have we crashed into the snake?
  if (_snakeBody->contains(nextHead)) {
    _reset();
    return;
  }

  // have we crashed into the wall?
  if (nextHead.first == 0 || nextHead.first == kGridWidth - 1 || nextHead.second == 0 || nextHead.second == kGridHeight - 1) {
    _reset();
    return;
  }

  _snakeBody->pushFront(nextHead);
  if(nextHead == _food) {
    _food = _placeFood();
  } else {
    _snakeBody->popBack();
  }
}

auto Snake::getState() const -> GameState { return { *_snakeBody, _food }; };

auto Snake::_placeFood() const -> GridPoint {
  while(true) {
    // Generate a random point in the grid, avoiding the walls.
    auto p = GridPoint { sdl::random::rand(kGridWidth - 2) + 1, sdl::random::rand(kGridHeight - 2) + 1 };
    if( ! _snakeBody->contains(p) ) {
      return p;
    }
  }
}

void Snake::_reset() {
  _snakeBody->clear();
  _snakeBody->pushFront({kStartingX, kStartingY});
  _direction = kStartingDirection;
  _nextDirection = kStartingDirection;
  _food = _placeFood();
}

}
