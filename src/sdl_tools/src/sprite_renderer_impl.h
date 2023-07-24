#ifndef __SDL_TOOLS_SPRITE_RENDERER_IMPL_H__
#define __SDL_TOOLS_SPRITE_RENDERER_IMPL_H__

#include "sprite_renderer.h"

namespace sdl::tools {

class SpriteRendererImpl {
  friend SpriteRenderer;
  public:
    SpriteRendererImpl(const Renderer& renderer): _renderer { renderer } {};

  private:
    const Renderer& _renderer;  
};

}

#endif
