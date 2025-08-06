#ifndef __SDL_SURFACE_IMPL_H__
#define __SDL_SURFACE_IMPL_H__

#include <SDL3/SDL.h>

#include "surface.h"

namespace sdl {

class SurfaceImpl{
  friend Surface;
  private:
    SDL_Surface* _sdlSurface;
};

}

#endif
