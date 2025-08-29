#ifndef RENDERER_IMPL_H
#define RENDERER_IMPL_H

#include <SDL3/SDL.h>

#include "sdl/constexpr_map.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"

namespace sdl {

class RendererImpl {
  friend Renderer;
  friend TextureImpl;

public:
  [[nodiscard]] auto getSdlRenderer() const -> SDL_Renderer* { return _sdlRenderer; }

 private:
  // This is ownned by the Window which was passed in - i.e. not us or the
  // Renderer
  SDL_Window* _sdlWindow = nullptr;
  // Owning Renderer class owns the SDL_Renderer
  SDL_Renderer* _sdlRenderer = nullptr;
};

}  // namespace sdl

#endif
