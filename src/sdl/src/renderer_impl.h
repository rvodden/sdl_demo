#ifndef __RENDERER_IMPL_H__
#define __RENDERER_IMPL_H__

#include <SDL2/SDL.h>

#include "texture.h"
#include "renderer.h"
#include "constexpr_map.h"

namespace sdl {

static constexpr vodden::Map<Renderer::RendererFlag, uint32_t, 4> sdlRendererFlagMap {{
    {Renderer::kSoftware, SDL_RENDERER_SOFTWARE},
    {Renderer::kAccelerated, SDL_RENDERER_ACCELERATED},
    {Renderer::kPresentVSync, SDL_RENDERER_PRESENTVSYNC},
    {Renderer::kTargetTexture, SDL_RENDERER_TARGETTEXTURE}
}};

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
