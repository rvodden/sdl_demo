#ifndef __SDL_SDL_IMPL_H__
#define __SDL_SDL_IMPL_H__

#include "sdl.h"

#include <unordered_set>

namespace sdl {

class SDLImpl {
  friend SDL;
  private:
    std::unordered_set<SDL::SubSystem> subSystemInitializationStatus {  };
};

}

#endif
