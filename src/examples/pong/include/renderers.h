#ifndef RENDERERS_H
#define RENDERERS_H

#include <memory>
#include <string>

#include "sdl/rectangle.h"
#include "sdl/text.h"
#include "sdl/renderer.h"
#include "sdl/font.h"
#include "sdl/texture.h"
#include "point.h"

template<typename T>
concept Rectangular = requires(T a) {
  { a.getExtent() } -> std::same_as<sdl::Rectangle<float>>;
};

template<Rectangular T> 
void renderRectangle(sdl::Renderer& renderer, const T& rectangle) {
  const auto extent = rectangle.getExtent();
  renderer.fillRect(extent);
}

class ScoreRenderer {
  public:
    ScoreRenderer(uint16_t score, std::shared_ptr<sdl::ttf::Font> font, std::shared_ptr<sdl::Renderer> renderer, Point<float> position) 
      : _score(score), _font(font), _renderer(renderer), _texture(_renderScore()), _position(position) {
      };
    
    [[nodiscard]] auto getScore() const -> uint16_t { return _score; }

    void setScore(uint16_t score) {
      if(score == _score) {
        return;
      }
      _score = score;
      _texture = _renderScore();
    }

    void render() {
      _renderer->copy(_texture, sdl::Rectangle<float>{_position.x, _position.y, 
        _texture.getSize().getWidth(), _texture.getSize().getHeight()});
    }

  private:
    auto _renderScore() -> sdl::Texture {
      auto surface = sdl::ttf::Text::renderBlended(*_font, std::to_string(_score), sdl::NamedColor::kWhite);
      return sdl::Texture(*_renderer, surface);
    }

    uint16_t _score;
    std::shared_ptr<sdl::ttf::Font> _font;
    std::shared_ptr<sdl::Renderer> _renderer;
    sdl::Texture _texture;
    Point<float> _position;
};

#endif  // RENDERERS_H
