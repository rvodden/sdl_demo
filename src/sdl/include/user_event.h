#ifndef __SDL_USER_EVENT_H__
#define __SDL_USER_EVENT_H__

#include "event.h"

namespace sdl {

class UserEventImpl;

class UserEvent: public Event {
  public:
    UserEvent();
    UserEvent(const UserEvent &userEvent);
    UserEvent(UserEvent &&userEvent);
    UserEvent& operator=(const UserEvent &userEvent);
    UserEvent& operator=(UserEvent &&userEvent);
    virtual ~UserEvent();
    // virtual void handle(BaseEventHandler &baseEventHandler) override {
    //   castHandler(*this, baseEventHandler);
    // };

  private:
    std::unique_ptr<UserEventImpl> _userEventImpl;
};

}

#endif
