#ifndef PONG_UI_H
#define PONG_UI_H

#include <memory>

#include <sdl/window.h>
#include <sdl/renderer.h>
#include <sdl/font.h>

#include "game_state.h"
#include "renderers.h"
#include "resources.h"
#include "constants.h"

namespace pong {

class PongUI {
 public:
  PongUI(Point<float> windowSize) : _windowSize(windowSize) {
    _window = std::make_unique<sdl::Window>("Pong", static_cast<uint16_t>(windowSize.x), static_cast<uint16_t>(windowSize.y), 0);
    _renderer = std::make_shared<sdl::Renderer>(*_window);
    _font = std::make_shared<sdl::ttf::Font>(&_binary_PressStart2P_Regular_ttf_start, pressStart2PSize(), kScoreFontSize);
    _scoreRenderer = std::make_unique<ScoreRenderer<kMaxScore>>(_font, _renderer);
  }

  template<GameState State>
  void render(const State& state) {
    _renderer->setDrawColour(sdl::NamedColor::kBlack);
    _renderer->clear();

    auto size = _window->getSize();
    _renderer->setDrawColour(sdl::NamedColor::kWhite);
    _renderer->drawLine(static_cast<float>(size.getWidth()) / 2, 0, static_cast<float>(size.getWidth()) / 2, static_cast<float>(size.getHeight()));

    renderRectangle(*_renderer, state.getBall());
    renderRectangle(*_renderer, state.getPaddle(Player::kLeft));
    renderRectangle(*_renderer, state.getPaddle(Player::kRight));

    _scoreRenderer->render(state.getScore(Player::kLeft),  Point<float>{ _windowSize.x * kScorePositionLeftRatio, kScoreVerticalOffset });
    _scoreRenderer->render(state.getScore(Player::kRight), Point<float>{ _windowSize.x * kScorePositionRightRatio, kScoreVerticalOffset });

    _renderer->present();
  }

 private:
  Point<float> _windowSize;
  std::unique_ptr<sdl::Window> _window;
  std::shared_ptr<sdl::Renderer> _renderer;
  std::shared_ptr<sdl::ttf::Font> _font;
  std::unique_ptr<ScoreRenderer<20>> _scoreRenderer;
};

} // namespace pong

#endif // PONG_UI_H
