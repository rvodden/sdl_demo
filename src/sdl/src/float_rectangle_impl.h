#ifndef SDL_RECTANGLE_IMPL_H
#define SDL_RECTANGLE_IMPL_H

#include <SDL3/SDL.h>

#include <cinttypes>
#include <stdfloat>

#include "float_rectangle.h"

namespace sdl {

class FloatRectangleImpl{
  friend FloatRectangle;
  public:
    FloatRectangleImpl() = default;
    FloatRectangleImpl(SDL_FRect sdlFRect) : _sdlFRect { sdlFRect } = default;
    SDL_FRect* getSDLRect() { return &_sdlFRect; };

  private:
    SDL_FRect _sdlFRect;
};

}

#endif
