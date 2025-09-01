#ifndef BALL_H
#define BALL_H

#include <cmath>
#include <stdexcept>

#include <sdl/rectangle.h>
#include <sdl/renderer.h>

#include "point.h"
#include "constants.h"

namespace pong {

class Ball {
  public:
    Ball(const Point<float>& initialPosition) : _initialPosition { initialPosition }, _extent( _initialExtent() ) {
      if (initialPosition.x < 0 || initialPosition.y < 0) {
        throw std::invalid_argument("Ball initial position cannot be negative");
      }
    }

    [[nodiscard]] auto getExtent() const -> sdl::Rectangle<float> {
      return _extent;
    }

    void resetToStartPositionAndVelocity() {
      _extent = _initialExtent();
      _velocity = kBallSpeed;
    }

    [[nodiscard]] auto getVelocity() const -> Point<float> { return _velocity; }
    void setVelocity(Point<float> velocity) { 
      if (std::abs(velocity.x) > kMaxBallSpeed || std::abs(velocity.y) > kMaxBallSpeed) {
        throw std::invalid_argument("Ball velocity exceeds maximum speed");
      }
      _velocity = velocity; 
    }

    void update(float dt) {
      _extent.setX(_extent.getX() + (_velocity.x * dt));
      _extent.setY(_extent.getY() + (_velocity.y * dt));
    }

  private:
    auto _initialExtent() const -> sdl::Rectangle<float> {
      return { _initialPosition.x - (kBallSize.x / 2), _initialPosition.y - (kBallSize.y / 2), kBallSize.x, kBallSize.y }; 
    } 
   
    Point<float> _initialPosition;
    sdl::Rectangle<float> _extent;
    Point<float> _velocity = kBallSpeed;
};

} // namespace pong

#endif  // BALL_H
