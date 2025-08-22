#ifndef SDL_USER_EVENT_IMPL_H
#define SDL_USER_EVENT_IMPL_H

#include <SDL3/SDL.h>

#include <optional>

#include "user_event.h"

namespace sdlpp {

class UserEvent;

class UserEventImpl {
  friend UserEvent;

 public:
  UserEventImpl(UserEvent* userEvent);
  auto _createSDLUserEvent() -> std::unique_ptr<SDL_Event>;

 private:
  UserEvent* _userEvent;
};

}  // namespace sdlpp

#endif
