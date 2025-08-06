#ifndef __SDL_TOOLS_EVENT_DISPATCHER_H__
#define __SDL_TOOLS_EVENT_DISPATCHER_H__

#include <memory>

#include "event.h"

namespace sdl::tools {

class EventDispatcherImpl;

class EventDispatcher {
  public:
    EventDispatcher(sdl::EventProducer& eventProducer);
    ~EventDispatcher();
    void run();
    void registerEventHandler(sdl::BaseEventHandler& baseEventHandler);

  private:
    std::unique_ptr<EventDispatcherImpl> _eventDispatcherImpl;
};

}

#endif
