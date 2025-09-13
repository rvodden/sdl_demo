#ifndef SNAKE_H
#define SNAKE_H

#include <array>
#include <cstdint>
#include <functional>
#include <utility>

#include <sdl/constexpr_map.h>
#include <sdl/event_router.h>

#include "constants.h"
#include "state.h"

namespace snake {

const auto kDirectionMapArray = std::array<std::pair<Direction, std::function<GridPoint(const GridPoint&)>>, 4>{
  std::pair{Direction::kUp,    [](const GridPoint& p) -> GridPoint { return {p.first, p.second - 1};} },
  std::pair{Direction::kDown,  [](const GridPoint& p) -> GridPoint { return {p.first, p.second + 1};} },
  std::pair{Direction::kLeft,  [](const GridPoint& p) -> GridPoint { return {p.first - 1, p.second};} },
  std::pair{Direction::kRight, [](const GridPoint& p) -> GridPoint { return {p.first + 1, p.second};} }
};

const auto kDirectionMap = sdl::Map(kDirectionMapArray);

class Snake {
  public:
    Snake(std::shared_ptr<sdl::tools::EventRouter> eventRouter);

    void update();
    [[nodiscard]] auto getState() const -> GameState;

  private:
    std::shared_ptr<sdl::tools::EventRouter> _eventRouter;

    std::unique_ptr<SnakeBody> _snakeBody = std::make_unique<SnakeBody>(std::initializer_list<GridPoint>{{kStartingX, kStartingY}});
    Direction _direction = kStartingDirection;
    Direction _nextDirection = kStartingDirection;
    GridPoint _food;

    [[nodiscard]] auto _placeFood() const -> GridPoint;
    void _reset();
};

}

#endif // SNAKE_H
