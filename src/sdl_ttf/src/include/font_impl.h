#ifndef SDL_TTF_FONT_IMPL_H
#define SDL_TTF_FONT_IMPL_H

#include <memory>

#include "sdl/font.h"

#include <SDL3_ttf/SDL_ttf.h>

namespace sdl::ttf {

class FontImpl {
  friend Font;
  friend Text;
  private:
    TTF_Font* _font;
};

}

#endif
