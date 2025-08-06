#ifndef __SDL_TOOLS_SPRITE_RENDERER_H__
#define __SDL_TOOLS_SPRITE_RENDERER_H__

#include <memory>

#include "renderer.h"

#include "sprite.h"

namespace sdl::tools {

class Sprite;
class SpriteRendererImpl;

class SpriteRenderer {
  public:
    SpriteRenderer(const Renderer& renderer);
    SpriteRenderer(SpriteRenderer&& other);
    ~SpriteRenderer();

    //! @brief render the provided sprite at the provided locations
    void render(const Sprite& sprite, const float x, const float y) const;
  
  private:
    std::unique_ptr<SpriteRendererImpl> _spriteRendererImpl;
};

}

#endif
