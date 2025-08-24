#ifndef SDL_COLOR_IMPL_H 
#define SDL_COLOR_IMPL_H

#include <color.h>

#include <SDL3/SDL.h>

#include "sdlpp_private_export.h"

namespace sdlpp {

auto constexpr createSDLColor(const Color& color) -> SDL_Color {
  return SDL_Color{
    .r = color.getRed(),
    .g = color.getGreen(),
    .b = color.getBlue(),
    .a = color.getAlpha()
  };
}

}


#endif
