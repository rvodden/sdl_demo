#include <memory>

#include "event_dispatcher_impl.h"

namespace sdl::tools {

void DefaultQuitEventHandler::handle( [[maybe_unused]] const QuitEvent& quitEvent ) {
  _eventDispatcherImpl.quit();
}

EventDispatcher::EventDispatcher( EventProducer& eventProducer ) : 
    _eventDispatcherImpl { std::make_unique<EventDispatcherImpl>( eventProducer ) }
{
  registerEventHandler( _eventDispatcherImpl->defaultQuitEventHandler );
}

EventDispatcher::~EventDispatcher() {};

void EventDispatcher::run() {
  std::unique_ptr<BaseEvent> event;
  while( !_eventDispatcherImpl->quitFlag ) {
    try {
      event = _eventDispatcherImpl->_eventProducer.wait();
      for(const auto& handler : _eventDispatcherImpl->_eventHandlers) {
        event->handle(handler);
      }
    } catch ([[maybe_unused]] sdl::UnknownEventException &_) { } // TODO: unknown events are "fine"
  }
}

void EventDispatcher::registerEventHandler(BaseEventHandler& baseEventHandler ) {
  _eventDispatcherImpl->_eventHandlers.push_front(std::ref(baseEventHandler));
}


}
