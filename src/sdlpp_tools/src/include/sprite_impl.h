#ifndef SPRITE_SHEET_IMPL_H
#define SPRITE_SHEET_IMPL_H

#include <memory>

#include "rectangle.h"
#include "sprite.h"

namespace sdlpp::tools {

class SpriteRenderer;

class SpriteImpl {
  friend Sprite;
  friend SpriteRenderer;

 public:
  SpriteImpl(std::shared_ptr<const Texture> texture, sdlpp::Rectangle<float> rectangle)
      : _spriteSheet{std::move(texture)}, _rectangle{std::move(rectangle)} {};

 private:
  std::shared_ptr<const Texture> _spriteSheet;
  sdlpp::Rectangle<float> _rectangle;
};

}  // namespace sdlpp::tools

#endif
