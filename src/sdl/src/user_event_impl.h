#ifndef SDL_USER_EVENT_IMPL_H
#define SDL_USER_EVENT_IMPL_H

#include <SDL3/SDL.h>

#include <optional>

#include "user_event.h"

namespace sdl {

class UserEvent;

class UserEventImpl {
  friend UserEvent;
  public:
    UserEventImpl();
    std::unique_ptr<SDL_Event> _createSDLUserEvent();
  
  private:
    UserEvent* _userEvent;
};

}

#endif
