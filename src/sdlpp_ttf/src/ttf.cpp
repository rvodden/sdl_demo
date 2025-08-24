#include <SDL3_ttf/SDL_ttf.h>

#include "ttf.h"

namespace sdlpp::ttf {

TTF::TTF() {
  TTF_Init();
}

TTF::~TTF() {
  TTF_Quit();
}

}
