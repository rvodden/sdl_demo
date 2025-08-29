#include <memory>

#include <SDL3_ttf/SDL_ttf.h>

#include "sdl/ttf.h"

// Service registration moved to header file for better dependency handling

namespace sdl::ttf {

TTF::TTF() {
  TTF_Init();
}

TTF::~TTF() {
  TTF_Quit();
}

// Service registration is now handled in ttf_application.h header

}
