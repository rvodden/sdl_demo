#ifndef PONG_UI_H
#define PONG_UI_H

#include <memory>

#include <sdl/window.h>
#include <sdl/rectangle.h>
#include <sdl/renderer.h>
#include <sdl/font.h>

#include "game_state.h"
#include "renderers.h"
#include "resources.h"
#include "constants.h"

namespace pong {

class PongUI;

class PongUIFactory {
 public:
  // Factory method for convenient creation
  static std::unique_ptr<PongUI> createPongUI(Point<float> initialWindowSize) {
    auto window = std::make_unique<sdl::Window>("Pong", 
                                               static_cast<uint16_t>(initialWindowSize.x), 
                                               static_cast<uint16_t>(initialWindowSize.y), 
                                               0);
    auto renderer = std::make_shared<sdl::Renderer>(*window);
    auto scoreRenderer = createScoreRenderer(renderer);
    
    return std::make_unique<PongUI>(std::move(window), renderer, std::move(scoreRenderer));
  }
  
  // Factory helper for font and score renderer creation
  static std::unique_ptr<ScoreRenderer<kMaxScore>> createScoreRenderer(
      std::shared_ptr<sdl::Renderer> renderer) {
    auto font = std::make_unique<sdl::ttf::Font>(&_binary_PressStart2P_Regular_ttf_start, 
                                                pressStart2PRegularSize(), 
                                                kScoreFontSize);
    return std::make_unique<ScoreRenderer<kMaxScore>>(std::move(font), renderer);
  }
};

class PongUI {
 public:
  // Constructor with dependency injection - follows SRP
  PongUI(std::unique_ptr<sdl::Window> window, 
         std::shared_ptr<sdl::Renderer> renderer,
         std::unique_ptr<ScoreRenderer<kMaxScore>> scoreRenderer)
    : _window(std::move(window))
    , _renderer(std::move(renderer))
    , _scoreRenderer(std::move(scoreRenderer)) {}

  template<GameState State>
  void render(const State& state) {
    _renderer->setDrawColour(sdl::NamedColor::kBlack);
    _renderer->clear();

    auto size = static_cast<sdl::Rectangle<float>>(_window->getSize());
    _renderer->setDrawColour(sdl::NamedColor::kWhite);
    _renderer->drawLine(size.getWidth() / 2, 0, size.getWidth() / 2, size.getHeight());

    renderRectangle(*_renderer, state.getBall());
    renderRectangle(*_renderer, state.getPaddle(Player::kLeft));
    renderRectangle(*_renderer, state.getPaddle(Player::kRight));

    _scoreRenderer->render(state.getScore(Player::kLeft),  Point<float>( size.getWidth() * kScorePositionLeftRatio, kScoreVerticalOffset ));
    _scoreRenderer->render(state.getScore(Player::kRight), Point<float>( size.getWidth() * kScorePositionRightRatio, kScoreVerticalOffset ));

    _renderer->present();
  }

 private:

  std::unique_ptr<sdl::Window> _window;
  std::shared_ptr<sdl::Renderer> _renderer;
  std::unique_ptr<ScoreRenderer<kMaxScore>> _scoreRenderer;
};

} // namespace pong

#endif // PONG_UI_H
