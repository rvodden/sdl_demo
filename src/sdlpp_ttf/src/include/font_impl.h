#ifndef SDLPP_TTF_FONT_IMPL_H
#define SDLPP_TTF_FONT_IMPL_H

#include <memory>

#include <font.h>

#include <SDL3_ttf/SDL_ttf.h>

namespace sdlpp::ttf {

class FontImpl {
  friend Font;
  friend Text;
  private:
    TTF_Font* _font;
};

}

#endif
