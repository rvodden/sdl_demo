#ifndef SDL_TOOLS_EVENT_DISPATCHER_IMPL_H
#define SDL_TOOLS_EVENT_DISPATCHER_IMPL_H

#include <atomic>
#include <forward_list>

#include "event_dispatcher.h"

namespace sdl::tools {

class EventDispatcher;
class EventDispatcherImpl;

class DefaultQuitEventHandler : public sdl::EventHandler<QuitEvent>, public sdl::BaseEventHandler {
  public:
    DefaultQuitEventHandler(EventDispatcherImpl& eventDispatcherImpl ) : _eventDispatcherImpl { eventDispatcherImpl } = default
    virtual void handle( [[maybe_unused]] const QuitEvent& quitEvent );
  private:
    EventDispatcherImpl& _eventDispatcherImpl;
};

class EventDispatcherImpl {
  friend EventDispatcher;
  public:
    EventDispatcherImpl( sdl::BaseEventProducer& eventProducer ) : _eventProducer { eventProducer } = default;
    void quit() { quitFlag = true; };
  private:
    sdl::BaseEventProducer& _eventProducer;
    std::forward_list<std::reference_wrapper<sdl::BaseEventHandler>> _eventHandlers = default;
    std::atomic_bool quitFlag { false };
    DefaultQuitEventHandler defaultQuitEventHandler { *this };
};

}

#endif
