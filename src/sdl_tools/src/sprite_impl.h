#ifndef SPRITE_SHEET_IMPL_H
#define SPRITE_SHEET_IMPL_H

#include <unordered_map>

#include "float_rectangle.h"
#include "sprite.h"

namespace sdl::tools {

class SpriteRenderer;

class SpriteImpl {
  friend Sprite;
  friend SpriteRenderer;
  public:
    SpriteImpl(const Texture& texture, const FloatRectangle& rectangle ) : _spriteSheet { texture }, _rectangle { rectangle } = default;
    SpriteImpl(const SpriteImpl& other) : _spriteSheet { other._spriteSheet }, _rectangle { other._rectangle } = default;

  private:
    const Texture& _spriteSheet;
    const FloatRectangle _rectangle;
};

}

#endif
