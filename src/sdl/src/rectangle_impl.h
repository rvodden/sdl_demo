#ifndef __SDL_RECTANGLE_IMPL_H__
#define __SDL_RECTANGLE_IMPL_H__

#include <SDL2/SDL.h>

#include <cinttypes>

#include "rectangle.h"

namespace sdl {

class RectangleImpl{
  friend Rectangle;
  public:
    RectangleImpl() = default;
    RectangleImpl(SDL_Rect sdlRect) : _sdlRect { sdlRect } {};
    SDL_Rect* getSDLRect() { return &_sdlRect; };

  private:
    SDL_Rect _sdlRect;
};

}

#endif
