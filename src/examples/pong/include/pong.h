#ifndef PONG_H
#define PONG_H

#include <array>
#include <stdexcept>

#include <sdl/event.h>
#include <sdl/event_router.h>

#include "ball.h"
#include "paddle.h"
#include "constants.h"

namespace pong {

class Pong {
 public:
  Pong(const Point<float>& windowSize,
       std::shared_ptr<sdl::tools::EventRouter> eventRouter)
      : _windowSize(windowSize),
        _ball(Point{windowSize.x / 2.0F, windowSize.y / 2.0F}),
        _paddles{Paddle(Point<float>{kPaddleFromWallDistance, windowSize.y / 2.0F}, 0, windowSize.y),
                 Paddle(Point<float>{windowSize.x - kPaddleFromWallDistance, windowSize.y / 2.0F}, 0, windowSize.y)},
        _scores{{Player::kLeft, 0}, {Player::kRight, 0}},
        _eventRouter(std::move(eventRouter)) {
    
    if (!_eventRouter) {
      throw std::invalid_argument("EventRouter cannot be null");
    }
    
    if (windowSize.x <= 0 || windowSize.y <= 0) {
      throw std::invalid_argument("Window size must be positive");
    }
    
    _registerEventHandlers();
  }

  void update(float dt) {
    // Validate delta time to prevent physics issues
    if (dt < kMinimumDt || dt > kMaximumDt ) {
      std::cerr << "Warning: Invalid delta time: " << dt << "ms, clamping to safe range\n";
      dt = std::clamp(dt, kMinimumDt, kMaximumDt);
    }

    _paddles.at(static_cast<size_t>(Player::kLeft)).update(dt);
    _paddles.at(static_cast<size_t>(Player::kRight)).update(dt);
    _ball.update(dt);

    const auto ballExtent = _ball.getExtent();
    auto ballVelocity = _ball.getVelocity();

    // Check Collisions between Ball and Walls
    if (ballVelocity.y < 0 && ballExtent.getY() <= 0) {
      _eventRouter->routeEvent(WallCollisionEvent{WallCollisionEvent::Wall::kTop});
    } else if (ballVelocity.y > 0 && ballExtent.getY() + ballExtent.getHeight() >= _windowSize.y) {
      _eventRouter->routeEvent(WallCollisionEvent{WallCollisionEvent::Wall::kBottom});
    }

    if (ballVelocity.x < 0) {  // moving left so might hit kLeft or the Left wall
      const auto& leftPaddle = _paddles.at(static_cast<size_t>(Player::kLeft));
      if (leftPaddle.checkCollision(ballExtent)) {
        _eventRouter->routeEvent(PaddleCollisionEvent{
            Player::kLeft, leftPaddle.determineCollisionZone(ballExtent)});
      } else if (ballExtent.getX() <= 0) {
        _eventRouter->routeEvent(WallCollisionEvent{WallCollisionEvent::Wall::kLeft});
      }
    }

    if (ballVelocity.x > 0) {  // moving right so might hit kRight
      const auto& rightPaddle = _paddles.at(static_cast<size_t>(Player::kRight));
      if (rightPaddle.checkCollision(ballExtent)) {
        _eventRouter->routeEvent(PaddleCollisionEvent{
            Player::kRight, rightPaddle.determineCollisionZone(ballExtent)});
      } else if (ballExtent.getX() + ballExtent.getWidth() >= _windowSize.x) {
        _eventRouter->routeEvent(WallCollisionEvent{WallCollisionEvent::Wall::kRight});
      }
    }
  }

  [[nodiscard]] auto getBall() const -> const Ball& { return _ball; }
  [[nodiscard]] auto getPaddle(Player player) const -> const Paddle& { return _paddles.at(static_cast<size_t>(player)); }
  [[nodiscard]] auto getScore(Player player) const -> uint16_t { return _scores.at(player); }

  void incrementScore(Player player) { _scores[player]++; }
  void setPaddleVelocity(Player player, Paddle::Velocity velocity) { _paddles.at(static_cast<size_t>(player)).setVelocity(velocity); }
  void setBallVelocity(const Point<float>& velocity) { _ball.setVelocity(velocity); }
  void setBallPosition(const Point<float>& position) { _ball.setPosition(position); }
  void resetBall() { _ball.resetToStartPositionAndVelocity(); }

 private:
  Point<float> _windowSize;

  // Actors
  Ball _ball;
  std::array<Paddle, 2> _paddles;
  std::map<Player, uint16_t> _scores;
  std::shared_ptr<sdl::tools::EventRouter> _eventRouter;

  void _registerEventHandlers() {
    // Left paddle controls - A key for up movement
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kA, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kLeft)).setVelocity(Paddle::Velocity::kUp);
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kA, sdl::KeyDirection::Up>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kLeft)).setVelocity(Paddle::Velocity::kStopped);
    });

    // Left paddle controls - Z key for down movement
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kZ, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kLeft)).setVelocity(Paddle::Velocity::kDown);
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kZ, sdl::KeyDirection::Up>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kLeft)).setVelocity(Paddle::Velocity::kStopped);
    });

    // Right paddle controls - L key for up movement
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kL, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kRight)).setVelocity(Paddle::Velocity::kUp);
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kL, sdl::KeyDirection::Up>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kRight)).setVelocity(Paddle::Velocity::kStopped);
    });

    // Right paddle controls - Comma key for down movement
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kComma, sdl::KeyDirection::Down>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kRight)).setVelocity(Paddle::Velocity::kDown);
    });
    
    _eventRouter->registerEventHandler<sdl::SpecificKeyboardEvent<sdl::KeyCode::kComma, sdl::KeyDirection::Up>>([this]([[maybe_unused]] const auto& event) -> void {
      _paddles.at(static_cast<size_t>(Player::kRight)).setVelocity(Paddle::Velocity::kStopped);
    });

    _eventRouter->registerEventHandler<PaddleCollisionEvent>(
        [this]([[maybe_unused]] const PaddleCollisionEvent& event) -> void {
          float velocityX = -_ball.getVelocity().x;
          float velocityY = 0;
          switch (event.zone) {
            case PaddleCollisionEvent::Zone::kTop:
              velocityY = kBallDeflectionAngle;
              break;
            case PaddleCollisionEvent::Zone::kMiddle:
              velocityY = 0;
              break;
            case PaddleCollisionEvent::Zone::kBottom:
              velocityY = -kBallDeflectionAngle;
              break;
            default:
              velocityY = 0;
              break;
          }
          _ball.setVelocity({velocityX, velocityY});
        });

    _eventRouter->registerEventHandler<WallCollisionEvent>([this](const WallCollisionEvent& event) -> void {
      switch (event.wall) {
        case WallCollisionEvent::Wall::kTop:
        case WallCollisionEvent::Wall::kBottom: {
          auto velocity = _ball.getVelocity();
          _ball.setVelocity({velocity.x, -velocity.y});
          break;
        }
        case WallCollisionEvent::Wall::kLeft: {
          _scores[Player::kRight] += 1;
          _ball.resetToStartPositionAndVelocity();
          _ball.setVelocity(kBallResetSpeed);
          break;
        }
        case WallCollisionEvent::Wall::kRight: {
          _scores[Player::kLeft] += 1;
          _ball.resetToStartPositionAndVelocity();
          _ball.setVelocity(-kBallResetSpeed);
          break;
        }
      }
    });
  }
};

} // namespace pong

#endif  // PONG_H
