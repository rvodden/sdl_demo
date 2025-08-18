#include <memory>

#include "event_dispatcher_impl.h"

namespace sdl::tools {

void DefaultQuitEventHandler::handle( [[maybe_unused]] const QuitEvent& quitEvent ) {
  _eventDispatcherImpl.get().quit();
}

EventDispatcher::EventDispatcher( std::shared_ptr<EventProducer> eventProducer ) : 
    _eventDispatcherImpl ( std::make_unique<EventDispatcherImpl>( std::move(eventProducer)) )
{
  registerEventHandler( _eventDispatcherImpl->defaultQuitEventHandler );
}

EventDispatcher::~EventDispatcher() = default;

void EventDispatcher::run() {
  std::unique_ptr<BaseEvent> event;
  while( !_eventDispatcherImpl->quitFlag ) {
    try {
      event = _eventDispatcherImpl->_eventProducer->wait();
      for(const auto& handler : _eventDispatcherImpl->_eventHandlers) {
        event->handle(handler);
      }
    } catch ([[maybe_unused]] sdl::UnknownEventException &_) { } // NOLINT(bugprone-empty-catch)
    // TODO: unknown events are "fine" whilst we've not implemented 
    // all the events. When we have coverage this catch should go.
  }
}

void EventDispatcher::registerEventHandler(BaseEventHandler& baseEventHandler ) {
  _eventDispatcherImpl->_eventHandlers.push_front(std::ref(baseEventHandler));
}


}
