#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

namespace pong {

enum class Player : uint8_t {
  kLeft,
  kRight
};

} // namespace pong

#endif // PLAYER_H
