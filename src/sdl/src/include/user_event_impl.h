#ifndef SDL_USER_EVENT_IMPL_H
#define SDL_USER_EVENT_IMPL_H

#include <optional>

#include <SDL3/SDL.h>

#include "sdl/user_event.h"

namespace sdl {

class UserEvent;

class UserEventImpl {
  friend UserEvent;

 public:
  UserEventImpl(UserEvent* userEvent);
  auto _createSDLUserEvent() -> std::unique_ptr<SDL_Event>;

 private:
  UserEvent* _userEvent;
};

}  // namespace sdl

#endif
