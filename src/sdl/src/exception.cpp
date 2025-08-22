#include "exception.h"

#include <SDL3/SDL.h>

namespace sdlpp {

Exception::Exception(const std::string& cause)
    : std::runtime_error(cause + " : " + SDL_GetError()),
      _cause{cause},
      _error{SDL_GetError()} {}

}  // namespace sdlpp
