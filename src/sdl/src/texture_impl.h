#ifndef __SDL_TEXTURE_IMPL_H__
#define __SDL_TEXTURE_IMPL_H__

#include <SDL2/SDL.h>

#include "renderer.h"
#include "texture.h"

namespace sdl {

static constexpr vodden::Map<Texture::BlendMode, SDL_BlendMode, 5> sdlBlendModeMap {{
    {Texture::kNone, SDL_BLENDMODE_NONE},
    {Texture::kBlend, SDL_BLENDMODE_BLEND},
    {Texture::kAdd, SDL_BLENDMODE_ADD},
    {Texture::kMod, SDL_BLENDMODE_MOD},
    {Texture::kMul, SDL_BLENDMODE_MUL}
}};

class TextureImpl {
  friend Renderer;
  friend Texture;
  private:
    SDL_Texture* _sdlTexture;
};

}

#endif

