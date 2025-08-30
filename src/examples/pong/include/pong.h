#ifndef PONG_H
#define PONG_H

#include <sdl/event.h>
#include <sdl/event_router.h>

#include "ball.h"
#include "paddle.h"

class Pong {
 public:
  Pong(const Point<float>& windowSize, std::shared_ptr<sdl::BaseEventBus> eventBus, std::shared_ptr<sdl::tools::EventRouter> eventRouter) : 
    _windowSize(windowSize),
    _ball ( std::make_shared<Ball>(Point{ windowSize.x / 2.F, windowSize.y / 2.F }) ),
    _paddles {
      { Player::kLeft, std::make_shared<Paddle>(Point<float>{ 50.F, windowSize.y / 2.F }, 0, windowSize.y ) },
      { Player::kRight, std::make_shared<Paddle>(Point<float>{ windowSize.x - 50.F, windowSize.y / 2.F }, 0, windowSize.y ) }
    },
    _scores { {Player::kLeft, 0}, {Player::kRight, 0} },
    _eventBus(eventBus),
    _eventRouter(eventRouter) {
      _registerEventHandlers();
  }

  void update(float dt) {
    _paddles[Player::kLeft]->update(dt);
    _paddles[Player::kRight]->update(dt);
    _ball->update(dt);

    const auto ballExtent = _ball->getExtent();

    // Check Collisions between Ball and Walls
    if(ballExtent.getY() <= 0) {
      _eventBus->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kTop));
    } else if (ballExtent.getY() + ballExtent.getHeight() >= _windowSize.y) {
      _eventBus->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kBottom));
    }

    if(ballExtent.getX() <= 0) {
      _eventBus->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kLeft));
    } else if( ballExtent.getX() + ballExtent.getWidth() >= _windowSize.x ) {
      _eventBus->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kRight));
    }

    // Check Collisions between Ball and Paddles
    auto ballVelocity = _ball->getVelocity();
    if(ballVelocity.x < 0) { // moving left so might hit kLeft
      if (_paddles[Player::kLeft]->checkCollision(ballExtent)) {
        _eventBus->publish(std::make_unique<PaddleCollisionEvent>(Player::kLeft, _paddles[Player::kLeft]->determineCollisionZone(ballExtent)));
      }
    }
    if(ballVelocity.x > 0) { // moving right so might hit kRight
      if (_paddles[Player::kRight]->checkCollision(ballExtent)) {
        _eventBus->publish(std::make_unique<PaddleCollisionEvent>(Player::kRight, _paddles[Player::kRight]->determineCollisionZone(ballExtent)));
      }
    }
  }

  [[nodiscard]] auto getBall() const -> std::shared_ptr<Ball> { return _ball; }
  [[nodiscard]] auto getPaddle(Player player) const -> std::shared_ptr<Paddle> { return _paddles.at(player); }
  [[nodiscard]] auto getScore(Player player) const -> uint16_t { return _scores.at(player); }

  void incrementScore(Player player) { _scores[player]++; }
  void setPaddleVelocity(Player player, Paddle::Velocity velocity) { _paddles[player]->setVelocity(velocity); }
  void resetBall() { _ball->reset(); }

 private:
  Point<float> _windowSize;

  // Actors
  std::shared_ptr<Ball> _ball;
  std::map<Player, std::shared_ptr<Paddle>> _paddles;
  std::map<Player, uint16_t> _scores;
  std::shared_ptr<sdl::BaseEventBus> _eventBus;
  std::shared_ptr<sdl::tools::EventRouter> _eventRouter;
  
  void _registerEventHandlers() {
    _eventRouter->registerEventHandler<sdl::KeyboardEvent>(
      [=,this](const sdl::KeyboardEvent& event) {
        switch(event.keycode) {
          case sdl::KeyboardEvent::KeyCode::kA:
            _paddles[Player::kLeft]->setVelocity(event.down ? Paddle::Velocity::kUp : Paddle::Velocity::kStopped);
            break;
          case sdl::KeyboardEvent::KeyCode::kZ:
            _paddles[Player::kLeft]->setVelocity(event.down ? Paddle::Velocity::kDown : Paddle::Velocity::kStopped);
            break;
          case sdl::KeyboardEvent::KeyCode::kL:
            _paddles[Player::kRight]->setVelocity(event.down ? Paddle::Velocity::kUp : Paddle::Velocity::kStopped);
            break;
          case sdl::KeyboardEvent::KeyCode::kComma:
            _paddles[Player::kRight]->setVelocity(event.down ? Paddle::Velocity::kDown : Paddle::Velocity::kStopped);
            break;
          default:
            break;
        }
      }
    );
    
    _eventRouter->registerEventHandler<PaddleCollisionEvent>(
      [=,this]([[maybe_unused]] const PaddleCollisionEvent& event) {
        float velocityX = -_ball->getVelocity().x;
        float velocityY;
        switch(event.zone) {
          case PaddleCollisionEvent::Zone::kTop:
            velocityY = - 0.325F; // TODO: be cleverer about this.
            break;
          case PaddleCollisionEvent::Zone::kMiddle:
            velocityY = 0;
            break;
          case PaddleCollisionEvent::Zone::kBottom:
            velocityY = 0.325F; // TODO: be cleverer about this.
            break;
          default:
            velocityY = 0;
            break;
        }
        _ball->setVelocity({velocityX, velocityY});
      }
    );

    _eventRouter->registerEventHandler<WallCollisionEvent>(
      [=,this](const WallCollisionEvent& event) {
        switch(event.wall) {
          case WallCollisionEvent::Wall::kTop:
          case WallCollisionEvent::Wall::kBottom: {
            auto velocity = _ball->getVelocity();
            _ball->setVelocity({velocity.x, -velocity.y});
            break;
          }
          case WallCollisionEvent::Wall::kLeft: {
            _scores[Player::kRight] += 1;
            _ball->reset();
            _ball->setVelocity({0.5F, 0}); // TODO: be cleverer about this.
            break;
          }
          case WallCollisionEvent::Wall::kRight: {
            _scores[Player::kLeft] += 1;
            _ball->reset();
            _ball->setVelocity({-0.5F, 0}); // TODO: be cleverer about this.
            break;
          }
        }
      }
    );
  }
};

#endif  // PONG_H
