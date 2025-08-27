#ifndef TICTACTOE_EVENTS_H
#define TICTACTOE_EVENTS_H

#include "game_constants.h"
#include <user_event.h>

#include <chrono>
#include <cstdint>

class ClickEvent : public sdl::CustomUserEvent<ClickEvent> {
 public:
  ClickEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
             uint8_t xParam, uint8_t yParam)
      : CustomUserEvent(ts, winId), x(xParam), y(yParam) {};

  uint8_t x;
  uint8_t y;
};

class GameCompletedEvent : public sdl::CustomUserEvent<GameCompletedEvent> {
 public:
  GameCompletedEvent(const GameState& state)
      : CustomUserEvent(std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch()), 0), _state(state) {}
  [[nodiscard]] auto getState() const noexcept -> GameState { return _state; }
 
 private:
  GameState _state;
};

class StartNewGameEvent : public sdl::CustomUserEvent<StartNewGameEvent> {
 public:
  StartNewGameEvent() : CustomUserEvent(std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch()), 0) {};
};

#endif // TICTACTOE_EVENTS_H
