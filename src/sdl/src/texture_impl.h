#ifndef SDL_TEXTURE_IMPL_H
#define SDL_TEXTURE_IMPL_H

#include <SDL3/SDL.h>

#include "constexpr_map.h"
#include "renderer.h"
#include "texture.h"

namespace sdl {

static constexpr auto sdlBlendModeMapData = std::array{
    std::pair{Texture::kNone, SDL_BLENDMODE_NONE},
    std::pair{Texture::kBlend, SDL_BLENDMODE_BLEND},
    std::pair{Texture::kAdd, SDL_BLENDMODE_ADD},
    std::pair{Texture::kMod, SDL_BLENDMODE_MOD},
    std::pair{Texture::kMul, SDL_BLENDMODE_MUL}
};

static constexpr auto sdlBlendModeMap = vodden::Map(sdlBlendModeMapData);

class TextureImpl {
  friend Renderer;
  friend Texture;
  private:
    SDL_Texture* _sdlTexture;
};

}

#endif

