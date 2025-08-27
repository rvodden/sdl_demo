#ifndef TICTACTOE_TICTACTOE_H
#define TICTACTOE_TICTACTOE_H

#include "events.h"
#include "game_constants.h"
#include <event.h>

#include <array>
#include <memory>
#include <optional>

class TicTacToe {
 public:
  enum class Player : std::uint8_t { kO,
                                     kX };

  TicTacToe(std::shared_ptr<sdl::EventBus> eventBus);

  [[nodiscard]] auto getCellState(const uint8_t& x, const uint8_t& y) const
      -> std::optional<Player>;

  void play(uint8_t x, uint8_t y);

 private:
  std::shared_ptr<sdl::EventBus> _eventBus;
  std::array<std::optional<Player>,
             static_cast<std::size_t>(kCellColumns* kCellRows)>
      _cells{std::nullopt, std::nullopt, std::nullopt,
             std::nullopt, std::nullopt, std::nullopt,
             std::nullopt, std::nullopt, std::nullopt};
  Player _turn{Player::kO};

  static auto _index(uint8_t x, uint8_t y) -> uint8_t;
  [[nodiscard]] auto _checkWinCondition() const -> GameState;
  [[nodiscard]] auto _checkRows() const -> GameState;
  [[nodiscard]] auto _checkColumns() const -> GameState;
  [[nodiscard]] auto _checkDiagonals() const -> GameState;
  [[nodiscard]] auto _checkForDraw() const -> GameState;
};

#endif // TICTACTOE_TICTACTOE_H