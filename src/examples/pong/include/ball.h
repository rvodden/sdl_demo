#ifndef BALL_H
#define BALL_H

#include <sdl/rectangle.h>
#include <sdl/renderer.h>

#include "point.h"

const auto kBallSize = Point<float> { 15.F, 15.F };
const auto kBallSpeed = Point<float> {-0.5F, 0.F };

class Ball {
  public:
    Ball(const Point<float>& initialPosition) : _initialPosition { initialPosition }, _extent( _initialExtent() ) {}

    auto getExtent() const -> sdl::Rectangle<float> {
      return _extent;
    }

    void reset() {
      _extent = _initialExtent();
      _velocity = kBallSpeed;
    }

    auto getVelocity() const -> Point<float> { return _velocity; }
    void setVelocity(Point<float> velocity) { _velocity = velocity; }

    void update(float dt) {
      _extent.setX(_extent.getX() + (_velocity.x * dt));
      _extent.setY(_extent.getY() + (_velocity.y * dt));
    }

  private:
    auto _initialExtent() -> sdl::Rectangle<float> {
      return { _initialPosition.x - (kBallSize.x / 2), _initialPosition.y - (kBallSize.y / 2), kBallSize.x, kBallSize.y }; 
    } 
   
    Point<float> _initialPosition;
    sdl::Rectangle<float> _extent;
    Point<float> _velocity = kBallSpeed;
};

#endif  // BALL_H
