#include <format>
#include <stdexcept>
#include <utility>

#include "tictactoe.h"
#include "event.h"
#include "events.h"
#include "user_event.h"

TicTacToe::TicTacToe(std::shared_ptr<sdl::BaseEventBus> eventBus)
    : _eventBus(std::move(eventBus)) {}

[[nodiscard]] auto TicTacToe::getCellState(const uint8_t& x, const uint8_t& y) const
    -> std::optional<Player> {
  return _cells.at(_index(x, y));
}

void TicTacToe::play(uint8_t x, uint8_t y) {
  const auto index = _index(x, y);
  if (_cells.at(index)) {
    return;
  }

  _cells.at(index) = {_turn};
  _turn = _turn == Player::kO ? Player::kX : Player::kO;
  
  const auto state = _checkWinCondition();
  if(state != GameState::kPlaying) {
    _eventBus->publish(std::make_unique<GameCompletedEvent>(state));
    return;
  }
  
  _publishTurnEvent();
}

void TicTacToe::reset() {
  _cells.fill(std::nullopt);
  _publishTurnEvent();
}

auto TicTacToe::_index(uint8_t x, uint8_t y) -> uint8_t {
  if (x > 2 || y > 2) {
    throw std::runtime_error(
        std::format("Cell coordinates are out of bounds: ({},{})", x, y));
  }
  return x + static_cast<uint8_t>(y * 3);
}

void TicTacToe::_publishTurnEvent() const {
  switch(_turn) {
    case Player::kO:
      _eventBus->publish(std::make_unique<PlayerOTurnEvent>());
      break; 
    case Player::kX:
      _eventBus->publish(std::make_unique<PlayerXTurnEvent>());
      break; 
  }
}

[[nodiscard]] auto TicTacToe::_checkWinCondition() const -> GameState {
  using enum GameState;
  auto state = _checkRows();
  if (state != kPlaying) {
    return state;
  }

  state = _checkColumns();
  if (state != kPlaying) {
    return state;
  }

  state = _checkDiagonals();
  if (state != kPlaying) {
    return state;
  }

  state = _checkForDraw();
  if (state != kPlaying) {
    return state;
  }

  return kPlaying;
}

[[nodiscard]] auto TicTacToe::_checkRows() const -> GameState {
  using enum GameState;
  for (uint8_t row = 0; row < 3; ++row) {
    if (_cells.at(_index(0, row)) &&
        _cells.at(_index(0, row)) == _cells.at(_index(1, row)) &&
        _cells.at(_index(1, row)) == _cells.at(_index(2, row))) {
      return _cells.at(_index(0, row)) == Player::kO ? kPlayerOWins
                                                    : kPlayerXWins;
    }
  }
  return kPlaying;
}

[[nodiscard]] auto TicTacToe::_checkColumns() const -> GameState {
  using enum GameState;
  for (uint8_t col = 0; col < 3; ++col) {
    if (_cells.at(_index(col, 0)) &&
        _cells.at(_index(col, 0)) == _cells.at(_index(col, 1)) &&
        _cells.at(_index(col, 1)) == _cells.at(_index(col, 2))) {
      return _cells.at(_index(col, 0)) == Player::kO ? kPlayerOWins
                                                    : kPlayerXWins;
    }
  }
  return kPlaying;
}

[[nodiscard]] auto TicTacToe::_checkDiagonals() const -> GameState {
  using enum GameState;
  // Check diagonals
  if (_cells.at(_index(0, 0)) &&
      _cells.at(_index(0, 0)) == _cells.at(_index(1, 1)) &&
      _cells.at(_index(1, 1)) == _cells.at(_index(2, 2))) {
    return _cells.at(_index(0, 0)) == Player::kO ? kPlayerOWins
                                                : kPlayerXWins;
  }

  if (_cells.at(_index(2, 0)) &&
      _cells.at(_index(2, 0)) == _cells.at(_index(1, 1)) &&
      _cells.at(_index(1, 1)) == _cells.at(_index(0, 2))) {
    return _cells.at(_index(2, 0)) == Player::kO ? kPlayerOWins
                                                : kPlayerXWins;
  }

  return kPlaying;
}

[[nodiscard]] auto TicTacToe::_checkForDraw() const -> GameState {
  // Check for draw (all cells filled)
  bool allFilled = true;
  for (const auto& cell : _cells) {
    if (!cell) {
      allFilled = false;
      break;
    }
  }

  return allFilled ? GameState::kDraw : GameState::kPlaying;
}
