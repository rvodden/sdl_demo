#ifndef SDL_TOOLS_SPRITE_RENDERER_IMPL_H
#define SDL_TOOLS_SPRITE_RENDERER_IMPL_H

#include "sprite_renderer.h"

namespace sdl::tools {

class SpriteRendererImpl {
  friend SpriteRenderer;
  public:
    SpriteRendererImpl(const Renderer& renderer): _renderer { renderer } = default;

  private:
    const Renderer& _renderer;  
};

}

#endif
