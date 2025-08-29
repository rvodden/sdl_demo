#ifndef SDL_TTF_TEXT_IMPL_H  
#define SDL_TTF_TEXT_IMPL_H   

#include <SDL3_ttf/SDL_ttf.h>

#include "sdl/text.h"

namespace sdl::ttf {

class TextImpl {
  friend Text;
 private:
  TTF_Text* _text = nullptr;
};

}

#endif
