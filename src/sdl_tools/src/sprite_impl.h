#ifndef __SPRITE_SHEET_IMPL_H__
#define __SPRITE_SHEET_IMPL_H__

#include <unordered_map>

#include "rectangle.h"
#include "sprite.h"

namespace sdl::tools {

class SpriteRenderer;

class SpriteImpl {
  friend Sprite;
  friend SpriteRenderer;
  public:
    SpriteImpl(const Texture& texture, const Rectangle& rectangle ) : _texture { texture }, _rectangle { rectangle } {};
    SpriteImpl(const SpriteImpl& other) : _texture { other._texture }, _rectangle { other._rectangle } {};

  private:
    const Texture& _texture;
    const Rectangle _rectangle;
};

}

#endif
