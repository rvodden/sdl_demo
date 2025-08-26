#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

#include "ttf.h"

// Service registration moved to header file for better dependency handling

namespace sdlpp::ttf {

TTF::TTF() {
  TTF_Init();
}

TTF::~TTF() {
  TTF_Quit();
}

// Service registration is now handled in ttf_application.h header

}
