#ifndef __SDL_SDL_IMPL_H__
#define __SDL_SDL_IMPL_H__

#include "sdl.h"
#include "constexpr_map.h"

#include <unordered_set>

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

class SDLImpl {
  friend SDL;
  private:
    std::unordered_set<SDL::SubSystem> subSystemInitializationStatus {  };
};

}

#endif
