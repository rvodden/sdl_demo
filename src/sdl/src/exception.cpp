#include <SDL2/SDL.h>

#include "exception.h"

namespace sdl {

Exception::Exception(std::string cause) : std::runtime_error(cause + " : " + SDL_GetError()), _cause{ cause }, _error { SDL_GetError() } {}

}
