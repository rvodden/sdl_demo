#ifndef PADDLE_H
#define PADDLE_H

#include <stdexcept>

#include <sdl/rectangle.h>

#include "point.h"
#include "constants.h"
#include "events.h"

namespace pong {

class Paddle {
  public:
    enum class Velocity : uint8_t {
      kUp,
      kDown,
      kStopped
    };

    Paddle(const Point<float>& initialPosition, float minY, float maxY, Point<float> size = kPaddleSize) 
      : _initialPosition(initialPosition), _size(size), _extent(_initialExtents()), _minY(minY), _maxY(maxY - size.y) {
      
      if (initialPosition.x < 0 || initialPosition.y < 0) {
        throw std::invalid_argument("Paddle initial position cannot be negative");
      }
      
      if (minY < 0 || maxY < 0 || maxY <= minY) {
        throw std::invalid_argument("Invalid paddle movement bounds");
      }
      
      if (size.x <= 0 || size.y <= 0) {
        throw std::invalid_argument("Paddle size must be positive");
      }
    }

    auto getExtent() const -> sdl::Rectangle<float> {
      return _extent;
    }

    void setVelocity(Velocity velocity) { _velocity = velocity; }

    void update(float dt) {
      switch (_velocity) {
        case Velocity::kUp:
          _extent.setY(_extent.getY() - (kPaddleSpeed * dt));
          if (_extent.getY() < _minY) {
            _extent.setY(_minY);
          }
          break;
        case Velocity::kDown:
          _extent.setY(_extent.getY() + (kPaddleSpeed * dt));
          if (_extent.getY() > _maxY) {
            _extent.setY(_maxY);
          }
          break;
        case Velocity::kStopped:
          break;
      }
    }

    bool checkCollision(const sdl::Rectangle<float>& other) const {
      return _extent.hasIntersection(other);
    }

    PaddleCollisionEvent::Zone determineCollisionZone(const sdl::Rectangle<float>& ballExtent) const {
      float paddleTop = _extent.getY();
      float paddleThird = _extent.getHeight() / kPaddleCollisionZoneDivisions;

      float ballCenterY = ballExtent.getY() + (ballExtent.getHeight() / 2.0f);

      if (ballCenterY < paddleTop + paddleThird) {
        return PaddleCollisionEvent::Zone::kTop;
      } else if (ballCenterY < paddleTop + 2 * paddleThird) {
        return PaddleCollisionEvent::Zone::kMiddle;
      } else {
        return PaddleCollisionEvent::Zone::kBottom;
      }
    }

  private:
    auto _initialExtents() -> sdl::Rectangle<float> {
      return { _initialPosition.x - (_size.x / 2), _initialPosition.y - (_size.y / 2), _size.x, _size.y }; 
    }

    Point<float> _initialPosition;
    Point<float> _size;
    sdl::Rectangle<float> _extent;
    Velocity _velocity = Velocity::kStopped;
    float _minY;
    float _maxY;
};

} // namespace pong

#endif  // PADDLE_H
