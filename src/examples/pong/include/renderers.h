#ifndef RENDERERS_H
#define RENDERERS_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <ranges>

#include "sdl/rectangle.h"
#include "sdl/text.h"
#include "sdl/renderer.h"
#include "sdl/font.h"
#include "sdl/texture.h"

#include "point.h"

namespace pong {

template<typename T>
concept Rectangular = requires(T a) {
  { a.getExtent() } -> std::convertible_to<sdl::Rectangle<float>>;
};

template<Rectangular T> 
void renderRectangle(sdl::Renderer& renderer, const T& rectangle) {
  const auto extent = rectangle.getExtent();
  renderer.fillRect(extent);
}

template<typename T>
concept Scoreable = requires(T a) {
  { a.getScore() } -> std::convertible_to<uint16_t>;
};


void renderScore(sdl::Renderer& renderer, const uint16_t& score, const sdl::ttf::Font& font, Point<float> position) {
  auto surface = sdl::ttf::Text::renderBlended(font, std::to_string(score), sdl::NamedColor::kWhite);
  auto texture = sdl::Texture(renderer, surface);
  renderer.copy(texture, sdl::Rectangle<float>{position.x, position.y, static_cast<float>(texture.getSize().getWidth()), static_cast<float>(texture.getSize().getHeight())});
}

template<uint32_t maxScore>
class ScoreRenderer {
  public:
    ScoreRenderer(std::shared_ptr<sdl::ttf::Font> font, std::shared_ptr<sdl::Renderer> renderer)
      : _font(std::move(font)), _renderer(std::move(renderer)) {
      for (uint32_t i : std::views::iota(0U, maxScore + 1U)) {
        auto surface = sdl::ttf::Text::renderBlended(*_font, std::to_string(i), sdl::NamedColor::kWhite);
        _scoreTextures.at(i) = sdl::Texture(*_renderer, surface);
      }
    }
    
    void render(uint16_t score, Point<float> position) {
      const auto& texture = _scoreTextures.at(static_cast<size_t>(score));
      _renderer->copy(texture, sdl::Rectangle<float>{position.x, position.y, texture.getSize().getWidth(), texture.getSize().getHeight()});
    }

  private:
    std::array<sdl::Texture, maxScore + 1> _scoreTextures;
    std::shared_ptr<sdl::ttf::Font> _font;
    std::shared_ptr<sdl::Renderer> _renderer;
};

} // namespace pong

#endif  // RENDERERS_H
