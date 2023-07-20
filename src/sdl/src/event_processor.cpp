#include <SDL2/SDL.h>

#include <constexpr_map.h>

#include "event_processor.h"
#include "event_processor_impl.h"

namespace sdl {

EventProcessor::EventProcessor() : _eventProcessorImpl { std::make_unique<EventProcessorImpl>() } {
  registerEventHandler(kQuit, std::bind(&EventProcessor::defaultQuitEventHandler, this, std::placeholders::_1) );
}

EventProcessor::~EventProcessor() {};

void EventProcessor::run() {
  SDL_Event event;  
  while ( not _eventProcessorImpl->quitFlag ) {
    SDL_WaitEvent(&event);
    try {
      for ( const Handler& handler : _eventProcessorImpl->_eventHandlers[sdlEventTypeMap[event.type]] ) {
        handler(createQuitEvent((SDL_QuitEvent*)(&event)));
      }
    } catch (std::range_error& rangeError) {} // TODO: map all the event types
  }
}

void EventProcessor::registerEventHandler( const EventType& eventType, Handler handler ) {
  _eventProcessorImpl->_eventHandlers[eventType].push_back(handler);
}

void EventProcessor::defaultQuitEventHandler(const Event &)
{ 
  _eventProcessorImpl->quitFlag = true;
}

QuitEvent createQuitEvent(const SDL_QuitEvent* quitEvent) {
  QuitEvent retVal {
    quitEvent->type, 
    std::chrono::milliseconds( SDL_GetTicks64() )
  };
  return retVal;
}

}
