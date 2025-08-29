#ifndef SPRITE_SHEET_IMPL_H
#define SPRITE_SHEET_IMPL_H

#include <memory>

#include "sdl/rectangle.h"
#include "sdl/sprite.h"

namespace sdl::tools {

class SpriteRendererImpl;

class SpriteImpl {
  friend Sprite;
  friend SpriteRendererImpl;

 public:
  SpriteImpl(std::shared_ptr<const Texture> texture, sdl::Rectangle<float> rectangle)
      : _spriteSheet{std::move(texture)}, _rectangle{std::move(rectangle)} {};

  [[nodiscard]] auto getSpriteSheet() const -> const std::shared_ptr<const Texture>& { return _spriteSheet; }
  [[nodiscard]] auto getRectangle() const -> const sdl::Rectangle<float>& { return _rectangle; }

 private:
  std::shared_ptr<const Texture> _spriteSheet;
  sdl::Rectangle<float> _rectangle;
};

}  // namespace sdl::tools

#endif
