#ifndef EVENTS_H
#define EVENTS_H

#include <chrono>
#include <sdl/user_event.h>

#include "player.h"

namespace pong {

class PaddleCollisionEvent : public sdl::CustomUserEvent<PaddleCollisionEvent> {
 public:
  enum class Zone : uint8_t {
    kTop,
    kMiddle,
    kBottom
  };

  PaddleCollisionEvent(Player _player, Zone _zone = Zone::kMiddle) 
    : CustomUserEvent(std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch()), 0), player(_player), zone(_zone) {}
  
  Player player;
  Zone zone;
};

class WallCollisionEvent : public sdl::CustomUserEvent<WallCollisionEvent> {
 public:
  enum class Wall : uint8_t {
    kTop,
    kBottom,
    kLeft,
    kRight
  };

  WallCollisionEvent(Wall _wall) 
    : CustomUserEvent(std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch()), 0), wall(_wall) {}
  
  Wall wall;
};

} // namespace pong

#endif // EVENTS_H
