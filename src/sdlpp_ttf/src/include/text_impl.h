#ifndef SDLPP_TTF_TEXT_IMPL_H  
#define SDLPP_TTF_TEXT_IMPL_H   

#include <text.h>

#include <SDL3_ttf/SDL_ttf.h>

namespace sdlpp::ttf {

class TextImpl {
  friend Text;
 private:
  TTF_Text* _text = nullptr;
};

}

#endif
