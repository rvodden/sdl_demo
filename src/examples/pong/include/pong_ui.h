#ifndef PONG_UI_H
#define PONG_UI_H

#include <memory>

#include <sdl/window.h>
#include <sdl/renderer.h>
#include <sdl/font.h>

#include "pong.h"
#include "renderers.h"
#include "resources.h"

class PongUI {
 public:
  PongUI(Point<float> windowSize) : _windowSize(windowSize) {
    _window = std::make_unique<sdl::Window>("Pong", static_cast<uint16_t>(windowSize.x), static_cast<uint16_t>(windowSize.y), 0);
    _renderer = std::make_shared<sdl::Renderer>(*_window);
    _font = std::make_shared<sdl::ttf::Font>(&_binary_DejaVuSansMono_ttf_start, dejaVuSansMonoSize(), 40); 
  
    _scoreRenderers[Player::kLeft] = std::make_shared<ScoreRenderer>(0, _font, _renderer, Point<float>{ windowSize.x * 0.25F, 20.F });
    _scoreRenderers[Player::kRight] = std::make_shared<ScoreRenderer>(0, _font, _renderer, Point<float>{ windowSize.x * 0.75F, 20.F });
  }

  void update(const Pong& pong) {
    _scoreRenderers[Player::kLeft]->setScore(pong.getScore(Player::kLeft));
    _scoreRenderers[Player::kRight]->setScore(pong.getScore(Player::kRight));
  }

  void render(const Pong& pong) {
    _renderer->setDrawColour(sdl::NamedColor::kBlack);
    _renderer->clear();

    auto size = _window->getSize();
    _renderer->setDrawColour(sdl::NamedColor::kWhite);
    _renderer->drawLine(static_cast<float>(size.getWidth()) / 2, 0, static_cast<float>(size.getWidth()) / 2, static_cast<float>(size.getHeight()));

    renderRectangle(*_renderer, *pong.getBall());
    renderRectangle(*_renderer, *pong.getPaddle(Player::kLeft));
    renderRectangle(*_renderer, *pong.getPaddle(Player::kRight));
    _scoreRenderers[Player::kLeft]->render();
    _scoreRenderers[Player::kRight]->render();

    _renderer->present();
  }

 private:
  Point<float> _windowSize;
  std::unique_ptr<sdl::Window> _window;
  std::shared_ptr<sdl::Renderer> _renderer;

  std::shared_ptr<sdl::ttf::Font> _font;
  std::map<Player, std::shared_ptr<ScoreRenderer>> _scoreRenderers;

};

#endif // PONG_UI_H
