#ifndef __SDL_TOOLS_EVENT_DISPATCHER_IMPL_H__
#define __SDL_TOOLS_EVENT_DISPATCHER_IMPL_H__

#include <atomic>
#include <forward_list>

#include "event_dispatcher.h"

namespace sdl::tools {

class EventDispatcher;
class EventDispatcherImpl;

class DefaultQuitEventHandler : public sdl::EventHandler<QuitEvent>, public sdl::BaseEventHandler {
  public:
    DefaultQuitEventHandler(EventDispatcherImpl& eventDispatcherImpl ) : _eventDispatcherImpl { eventDispatcherImpl } {}
    virtual void handle( [[maybe_unused]] const QuitEvent& quitEvent );
  private:
    EventDispatcherImpl& _eventDispatcherImpl;
};

class EventDispatcherImpl {
  friend EventDispatcher;
  public:
    EventDispatcherImpl( sdl::BaseEventProducer& eventProducer ) : _eventProducer { eventProducer } {};
    void quit() { quitFlag = true; };
  private:
    sdl::BaseEventProducer& _eventProducer;
    std::forward_list<std::reference_wrapper<sdl::BaseEventHandler>> _eventHandlers {};
    std::atomic_bool quitFlag { false };
    DefaultQuitEventHandler defaultQuitEventHandler { *this };
};

}

#endif
