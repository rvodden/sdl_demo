#ifndef __SDL_TOOLS_SPRITE_SHEET_H__
#define __SDL_TOOLS_SPRITE_SHEET_H__

#include <memory>

#include "sprite_renderer.h"

#include "texture.h"

namespace sdl::tools{

class SpriteImpl;
class SpriteRenderer;

class Sprite {
  friend SpriteRenderer;
  public:
    Sprite(const Texture& texture, const Rectangle& rectangle);
    Sprite(Sprite&& other);

    ~Sprite();

  private:
    std::unique_ptr<SpriteImpl> _spriteImpl;
};

}

#endif
