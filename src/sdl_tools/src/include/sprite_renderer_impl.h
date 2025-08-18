#ifndef SDL_TOOLS_SPRITE_RENDERER_IMPL_H
#define SDL_TOOLS_SPRITE_RENDERER_IMPL_H

#include "sprite_renderer.h"

namespace sdl::tools {

class SpriteRendererImpl {
  friend SpriteRenderer;

 public:
  SpriteRendererImpl(std::shared_ptr<Renderer> renderer)
      : _renderer(std::move(renderer)) {};

 private:
  std::shared_ptr<Renderer> _renderer;
};

}  // namespace sdl::tools

#endif
