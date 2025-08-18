#ifndef SDL_SURFACE_IMPL_H
#define SDL_SURFACE_IMPL_H

#include <SDL3/SDL.h>

#include "surface.h"

namespace sdl {

class SurfaceImpl {
  friend Surface;

 private:
  SDL_Surface* _sdlSurface;
};

}  // namespace sdl

#endif
