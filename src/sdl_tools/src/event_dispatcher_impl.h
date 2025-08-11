#ifndef SDL_TOOLS_EVENT_DISPATCHER_IMPL_H
#define SDL_TOOLS_EVENT_DISPATCHER_IMPL_H

#include <atomic>
#include <forward_list>
#include <memory>

#include "event_dispatcher.h"

namespace sdl::tools {

class EventDispatcher;
class EventDispatcherImpl;

class DefaultQuitEventHandler : public sdl::EventHandler<QuitEvent>, public sdl::BaseEventHandler {
  public:
    DefaultQuitEventHandler(std::shared_ptr<EventDispatcherImpl> eventDispatcherImpl ) : _eventDispatcherImpl ( std::move(eventDispatcherImpl) ) {};
    void handle( [[maybe_unused]] const QuitEvent& quitEvent ) override;
  private:
    std::shared_ptr<EventDispatcherImpl> _eventDispatcherImpl;
};

class EventDispatcherImpl {
  friend EventDispatcher;
  public:
    EventDispatcherImpl( std::shared_ptr<EventProducer> eventProducer ) : _eventProducer ( std::move(eventProducer) ) {};
    void quit() { quitFlag = true; };
  private:
    std::shared_ptr<BaseEventProducer> _eventProducer;
    std::forward_list<std::reference_wrapper<sdl::BaseEventHandler>> _eventHandlers;
    std::atomic_bool quitFlag { false };
    DefaultQuitEventHandler defaultQuitEventHandler { std::shared_ptr<EventDispatcherImpl>(this) };
};

}

#endif
