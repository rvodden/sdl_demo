#ifndef SURFACE_IMPL_H
#define SURFACE_IMPL_H

#include "surface.h"

#include <SDL3/SDL.h>

namespace sdlpp {

class SurfaceImpl {
  friend class Surface;
  friend class TextureImpl;

  public:
    static auto createSurface(SDL_Surface* sdlSurface) -> Surface {
      auto surface = Surface();
      surface._impl->_sdlSurface = sdlSurface;

      return surface;
    }

    [[nodiscard]] auto getSdlSurface() const -> SDL_Surface* { return _sdlSurface; }

 private:
  // Owning Surface class owns the SDL_Surface
  SDL_Surface* _sdlSurface = nullptr;
};

}  // namespace sdlpp

#endif
