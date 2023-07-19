#include <memory>
#include <SDL2/SDL.h>

#include "constexpr_map.h"
#include "exception.h"

#include "sdl.h"
#include "sdl_impl.h"

namespace sdl {

static constexpr vodden::Map<SDL::SubSystem, uint32_t, 9> sdlSubSystemMap {{
  { SDL::kAudio, SDL_INIT_AUDIO },
  { SDL::kEvents, SDL_INIT_EVENTS },
  { SDL::kEverything, SDL_INIT_EVERYTHING },
  { SDL::kGameController, SDL_INIT_GAMECONTROLLER },
  { SDL::kHaptic, SDL_INIT_HAPTIC },
  { SDL::kJoystick, SDL_INIT_JOYSTICK },
  { SDL::kNoParachute, SDL_INIT_NOPARACHUTE },
  { SDL::kTimer, SDL_INIT_TIMER },
  { SDL::kVideo, SDL_INIT_VIDEO }
}};

SDL::SDL() {
  _sdlImpl = std::make_unique<SDLImpl>();
}

SDL::~SDL() noexcept {
  for (const auto& subSystem : _sdlImpl->subSystemInitializationStatus) {
    SDL_QuitSubSystem( sdlSubSystemMap[subSystem] );
  }
}

void SDL::initSubSystem(const SubSystem &subSystem) {
auto retVal = SDL_InitSubSystem(sdlSubSystemMap[subSystem]);
  if(retVal < 0) throw Exception("SDL_InitSubSystem");
  _sdlImpl->subSystemInitializationStatus.insert(subSystem);
}

void delay_ms(uint32_t duration) {
  SDL_Delay(duration);
}

}
