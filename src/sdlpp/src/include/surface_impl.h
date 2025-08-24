#ifndef SURFACE_IMPL_H
#define SURFACE_IMPL_H

#include "surface.h"

#include <SDL3/SDL.h>

namespace sdlpp {

class SurfaceImpl {
  friend class Surface;
  friend class Texture;

  public:
    static auto createSurface(SDL_Surface* sdlSurface) -> Surface {
      auto surface = Surface();
      surface._surfaceImpl->_sdlSurface = sdlSurface;

      return surface;
    }

 private:
  // Owning Surface class owns the SDL_Surface
  SDL_Surface* _sdlSurface = nullptr;
};

}  // namespace sdlpp

#endif
