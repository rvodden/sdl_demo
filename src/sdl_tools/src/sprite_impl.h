#ifndef __SPRITE_SHEET_IMPL_H__
#define __SPRITE_SHEET_IMPL_H__

#include <unordered_map>

#include "float_rectangle.h"
#include "sprite.h"

namespace sdl::tools {

class SpriteRenderer;

class SpriteImpl {
  friend Sprite;
  friend SpriteRenderer;
  public:
    SpriteImpl(const Texture& texture, const FloatRectangle& rectangle ) : _spriteSheet { texture }, _rectangle { rectangle } {};
    SpriteImpl(const SpriteImpl& other) : _spriteSheet { other._spriteSheet }, _rectangle { other._rectangle } {};

  private:
    const Texture& _spriteSheet;
    const FloatRectangle _rectangle;
};

}

#endif
