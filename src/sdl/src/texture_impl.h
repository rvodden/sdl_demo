#ifndef __SDL_TEXTURE_IMPL_H__
#define __SDL_TEXTURE_IMPL_H__

#include <SDL2/SDL.h>

#include "renderer.h"
#include "texture.h"

namespace sdl {

class TextureImpl {
  friend Renderer;
  friend Texture;
  private:
    SDL_Texture* _sdlTexture;
};

}

#endif

