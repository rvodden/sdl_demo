#ifndef SPRITE_SHEET_IMPL_H
#define SPRITE_SHEET_IMPL_H

#include <memory>

#include "rectangle.h"
#include "sprite.h"

namespace sdlpp::tools {

class SpriteRendererImpl;

class SpriteImpl {
  friend Sprite;
  friend SpriteRendererImpl;

 public:
  SpriteImpl(std::shared_ptr<const Texture> texture, sdlpp::Rectangle<float> rectangle)
      : _spriteSheet{std::move(texture)}, _rectangle{std::move(rectangle)} {};

  [[nodiscard]] auto getSpriteSheet() const -> const std::shared_ptr<const Texture>& { return _spriteSheet; }
  [[nodiscard]] auto getRectangle() const -> const sdlpp::Rectangle<float>& { return _rectangle; }

 private:
  std::shared_ptr<const Texture> _spriteSheet;
  sdlpp::Rectangle<float> _rectangle;
};

}  // namespace sdlpp::tools

#endif
