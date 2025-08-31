#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <concepts>

#include "ball.h"
#include "paddle.h"
#include "player.h"

namespace pong {

template<typename T>
concept GameState = requires(const T& t, Player player) {
  { t.getBall()} -> std::convertible_to<const Ball&>;
  { t.getPaddle(player) } -> std::convertible_to<const Paddle&>;
  { t.getScore(player) } -> std::convertible_to<uint16_t>;
};

} // namespace pong

#endif // GAME_STATE_H
