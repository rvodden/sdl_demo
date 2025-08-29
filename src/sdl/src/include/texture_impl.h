#ifndef SDL_TEXTURE_IMPL_H
#define SDL_TEXTURE_IMPL_H

#include <SDL3/SDL.h>

#include "sdl/constexpr_map.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"

namespace sdl {

static constexpr auto sdlBlendModeMapData =
    std::array{std::pair{Texture::kNone, SDL_BLENDMODE_NONE},
               std::pair{Texture::kBlend, SDL_BLENDMODE_BLEND},
               std::pair{Texture::kAdd, SDL_BLENDMODE_ADD},
               std::pair{Texture::kMod, SDL_BLENDMODE_MOD},
               std::pair{Texture::kMul, SDL_BLENDMODE_MUL}};

static constexpr auto sdlBlendModeMap = sdl::Map(sdlBlendModeMapData);

class TextureImpl {
  friend RendererImpl;
  friend Texture;

public:
  [[nodiscard]] auto getSdlTexture() const -> SDL_Texture* { return _sdlTexture; }

 private:
  SDL_Texture* _sdlTexture = nullptr;
};

}  // namespace sdl

#endif
