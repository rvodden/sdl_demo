#include <chrono>
#include <memory>
#include <map>
#include <iostream>

#include <sdl/application.h>
#include <sdl/rectangle.h>
#include <sdl/renderer.h>
#include <sdl/sdl.h>
#include <sdl/ttf_service.h>
#include <sdl/event_router.h>
#include <sdl/event.h>
#include <sdl/font.h>
#include <sdl/window.h>

#include "ball.h"
#include "events.h"
#include "paddle.h"
#include "player.h"
#include "renderers.h"
#include "resources.h"

const auto kWindowWidth = 1280;
const auto kWindowHeight = 720;

class Pong : public sdl::BaseApplication {
 public:
  auto init() -> bool override {
    auto& sdl = requestSDL();
    [[maybe_unused]] auto& ttf = requestService<sdl::ttf::TTF>();
    sdl.initSubSystem(sdl::SDL::kVideo);
    sdl.initSubSystem(sdl::SDL::kEvents);

    // Create infrastructure
    _window = std::make_unique<sdl::Window>("Pong", kWindowWidth, kWindowHeight, 0);
    _renderer = std::make_shared<sdl::Renderer>(*_window);
    _font = std::make_shared<sdl::ttf::Font>(&_binary_DejaVuSansMono_ttf_start, dejaVuSansMonoSize(), 40); 

    const auto windowSize = sdl::Rectangle<float>(_window->getSize());
    const auto windowCenter = Point<float>{ windowSize.getWidth(), windowSize.getHeight() } * 0.5F;
    
    // Create actors
    _ball = std::make_shared<Ball>(windowCenter);
    _paddles[Player::kLeft] = std::make_shared<Paddle>(Point<float>{ 50.F, windowCenter.y }, 0, kWindowHeight);
    _paddles[Player::kRight] = std::make_shared<Paddle>(Point<float>{ windowSize.getWidth() - 50.F, windowCenter.y }, 0, kWindowHeight);

    // CreateUI
    _ballRenderer = std::make_shared<RectangleRenderer<Ball>>(_ball, _renderer);
    _paddleRenderers[Player::kLeft] = std::make_shared<RectangleRenderer<Paddle>>(_paddles[Player::kLeft], _renderer);
    _paddleRenderers[Player::kRight] = std::make_shared<RectangleRenderer<Paddle>>(_paddles[Player::kRight], _renderer);
    _scoreRenderers[Player::kLeft] = std::make_shared<ScoreRenderer>(_scores[Player::kLeft], _font, _renderer, Point<float>{ windowSize.getWidth() * 0.25F, 20.F });
    _scoreRenderers[Player::kRight] = std::make_shared<ScoreRenderer>(_scores[Player::kRight], _font, _renderer, Point<float>{ windowSize.getWidth() * 0.75F, 20.F });

    _registerEventHandlers();
    return true;
  }

  auto iterate() -> bool override {
    // Get Time
    auto stopTime = std::chrono::high_resolution_clock::now();
	  auto dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
   
    // Update Actors
    _paddles[Player::kLeft]->update(dt);
    _paddles[Player::kRight]->update(dt);
    _ball->update(dt);

    const auto ballExtent = _ball->getExtent();
    if(ballExtent.getY() <= 0) {
      getEventBus()->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kTop));
    } else if (ballExtent.getY() + ballExtent.getHeight() >= static_cast<float>(kWindowHeight)) {
      getEventBus()->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kBottom));
    }

    if(ballExtent.getX() <= 0) {
      getEventBus()->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kLeft));
    } else if( ballExtent.getX() + ballExtent.getWidth() >= static_cast<float>(kWindowWidth)) {
      getEventBus()->publish(std::make_unique<WallCollisionEvent>(WallCollisionEvent::Wall::kRight));
    }

    // Check Collisions between Ball and Paddles
    auto eventBus = getEventBus();
    auto ballVelocity = _ball->getVelocity();
    if(ballVelocity.x < 0) { // moving left so might hit kLeft
      if (_paddles[Player::kLeft]->checkCollision(ballExtent)) {
        eventBus->publish(std::make_unique<PaddleCollisionEvent>(Player::kLeft, _paddles[Player::kLeft]->determineCollisionZone(ballExtent)));
      }
    }
    if(ballVelocity.x > 0) { // moving right so might hit kRight
      if (_paddles[Player::kRight]->checkCollision(ballExtent)) {
        eventBus->publish(std::make_unique<PaddleCollisionEvent>(Player::kRight, _paddles[Player::kRight]->determineCollisionZone(ballExtent)));
      }
    }
    
    // Update timer
    startTime = stopTime;

    // Render
    _renderer->setDrawColour(sdl::NamedColor::kBlack);
    _renderer->clear();

    auto size = _window->getSize();
    _renderer->setDrawColour(sdl::NamedColor::kWhite);
    _renderer->drawLine(static_cast<float>(size.getWidth()) / 2, 0, static_cast<float>(size.getWidth()) / 2, static_cast<float>(size.getHeight()));

    _ballRenderer->render();
    _paddleRenderers[Player::kLeft]->render();
    _paddleRenderers[Player::kRight]->render();
    _scoreRenderers[Player::kLeft]->render();
    _scoreRenderers[Player::kRight]->render();

    _renderer->present();
    return true;
  }

  auto quit() -> void override {
    // Cleanup if necessary
  }
 
 private:
  std::unique_ptr<sdl::Window> _window;
  std::shared_ptr<sdl::Renderer> _renderer;

  std::shared_ptr<sdl::ttf::Font> _font;

  std::shared_ptr<Ball> _ball;
  std::map<Player, std::shared_ptr<Paddle>> _paddles;
  std::map<Player, uint16_t> _scores = { {Player::kLeft, 0}, {Player::kRight, 0} };

  std::shared_ptr<RectangleRenderer<Ball>> _ballRenderer;
  std::map<Player, std::shared_ptr<RectangleRenderer<Paddle>>> _paddleRenderers;
  std::map<Player, std::shared_ptr<ScoreRenderer>> _scoreRenderers;

  std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

  void _registerEventHandlers() {
    auto eventRouter = getEventRouter();
    
    eventRouter->registerEventHandler<sdl::KeyboardEvent>(
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

    eventRouter->registerEventHandler<PaddleCollisionEvent>(
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

    eventRouter->registerEventHandler<WallCollisionEvent>(
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
            _scoreRenderers[Player::kRight]->setScore(_scores[Player::kRight]);
            _ball->reset();
            _ball->setVelocity({0.5F, 0}); // TODO: be cleverer about this.
            break;
          }
          case WallCollisionEvent::Wall::kRight: {
            _scores[Player::kLeft] += 1;
            _scoreRenderers[Player::kLeft]->setScore(_scores[Player::kLeft]);
            _ball->reset();
            _ball->setVelocity({-0.5F, 0}); // TODO: be cleverer about this.
            break;
          }
        }
      }
    );
  }
};

REGISTER_APPLICATION(Pong);
