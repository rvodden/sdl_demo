#ifndef SPRITE_SHEET_IMPL_H
#define SPRITE_SHEET_IMPL_H

#include <memory>

#include "float_rectangle.h"
#include "sprite.h"

namespace sdl::tools {

class SpriteRenderer;

class SpriteImpl {
  friend Sprite;
  friend SpriteRenderer;

 public:
  SpriteImpl(std::shared_ptr<const Texture> texture, FloatRectangle rectangle)
      : _spriteSheet{std::move(texture)}, _rectangle{std::move(rectangle)} {};

 private:
  std::shared_ptr<const Texture> _spriteSheet;
  FloatRectangle _rectangle;
};

}  // namespace sdl::tools

#endif
