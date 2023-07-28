#ifndef __SDL_USER_EVENT_IMPL_H__
#define __SDL_USER_EVENT_IMPL_H__

#include <SDL2/SDL.h>

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
    static std::optional<uint32_t> _eventType;
    UserEvent* _userEvent;
};

}

#endif
