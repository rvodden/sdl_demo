#ifndef SDL_SDL_IMPL_H
#define SDL_SDL_IMPL_H

#include <unordered_set>

#include "sdl/constexpr_map.h"
#include "sdl/sdl.h"

namespace sdl {

static constexpr sdl::Map<SDL::SubSystem, uint32_t, 6> sdlSubSystemMap{
    {{SDL::kAudio, SDL_INIT_AUDIO},
     {SDL::kEvents, SDL_INIT_EVENTS},
     {SDL::kGameController, SDL_INIT_GAMEPAD},
     {SDL::kHaptic, SDL_INIT_HAPTIC},
     {SDL::kJoystick, SDL_INIT_JOYSTICK},
     {SDL::kVideo, SDL_INIT_VIDEO}}};

class SDLImpl {
  friend SDL;

 private:
  std::unordered_set<SDL::SubSystem> subSystemInitializationStatus;
};

}  // namespace sdl

#endif
