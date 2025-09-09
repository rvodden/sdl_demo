#ifndef SNAKE_H
#define SNAKE_H

#include <utility>

#include "constants.h"
#include "state.h"

namespace snake {

class Snake {
  public:
    Snake() = default;

    [[nodiscard]] auto getState() const -> GameState { return { *_snakeBody, *_food }; };

  private:
    std::unique_ptr<SnakeBody> _snakeBody = std::make_unique<SnakeBody>(std::initializer_list<GridPoint>{{kStartingX, kStartingY}});
    std::unique_ptr<GridPoint> _food      = std::make_unique<GridPoint>();
};

}

#endif // SNAKE_H
