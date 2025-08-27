#include "sdl.h"

#include <SDL3/SDL.h>

#include <memory>

#include "exception.h"
#include "sdl_impl.h"

namespace sdl {

SDL::SDL() { _impl = std::make_unique<SDLImpl>(); }

SDL::~SDL() noexcept {
  for (const auto& subSystem : _impl->subSystemInitializationStatus) {
    SDL_QuitSubSystem(sdlSubSystemMap[subSystem]);
  }
}

void SDL::initSubSystem(const SubSystem& subSystem) {
  auto retVal = SDL_InitSubSystem(sdlSubSystemMap[subSystem]);
  if (!retVal) {
    throw Exception("SDL_InitSubSystem");
  }
  _impl->subSystemInitializationStatus.insert(subSystem);
}

void delay_ms(uint32_t duration) { SDL_Delay(duration); }

}  // namespace sdl
