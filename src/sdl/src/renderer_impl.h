#ifndef RENDERER_IMPL_H
#define RENDERER_IMPL_H

#include <SDL3/SDL.h>

#include "texture.h"
#include "renderer.h"
#include "constexpr_map.h"

namespace sdl {

class RendererImpl {
  friend Renderer;
  friend Texture;
  private:
    // This is ownned by the Window which was passed in - i.e. not us or the Renderer
    SDL_Window* _sdlWindow;
    // Owning Renderer class owns the SDL_Renderer
    SDL_Renderer* _sdlRenderer;

};

}

#endif
