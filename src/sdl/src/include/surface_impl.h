#ifndef SURFACE_IMPL_H
#define SURFACE_IMPL_H

#include <SDL3/SDL.h>

namespace sdl {

class SurfaceImpl {
  friend class Surface;

 private:
  // Owning Surface class owns the SDL_Surface
  SDL_Surface* _sdlSurface = nullptr;
};

}  // namespace sdl

#endif