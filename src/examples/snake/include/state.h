#ifndef STATE_H
#define STATE_H

#include <cstdint>
#include <memory>
#include <utility>

#include "linked_hash_deque.h"


namespace snake {

using GridPoint = std::pair<uint16_t, uint16_t>;

class GridPointHash {
  public:
    auto operator()(const GridPoint& p) const noexcept -> std::size_t {
      return static_cast<uint32_t>(p.first) | p.second;
    }
};

class GridPointEq {
  public:
    auto operator()(const GridPoint& lhs, const GridPoint& rhs) const noexcept -> bool {
      return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

using SnakeBody = LinkedHashDeque<GridPoint, GridPointHash, GridPointEq>;

class GameState {
  public:
    GameState(const SnakeBody& snakeBody, const GridPoint& food) : _snakeBody(snakeBody), _food(food) {}
    GameState(GameState&) = delete;
    GameState(GameState&&) = delete;

    ~GameState() = default;

    auto operator=(GameState&) -> GameState& = delete;
    auto operator=(GameState&&) noexcept -> GameState& = delete;

  private:
    const SnakeBody& _snakeBody;
    const GridPoint& _food;
};

}

#endif // STATE_H
