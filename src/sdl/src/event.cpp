#include <memory>

#include <SDL2/SDL.h>

#include <constexpr_map.h>

#include "event.h"
#include "event_impl.h"

namespace sdl {

void DefaultQuitEventHandler::handle( [[maybe_unused]] const QuitEvent& quitEvent ) {
  _eventProcessorImpl.quit();
}

EventProcessor::EventProcessor() : _eventProcessorImpl { std::make_unique<EventProcessorImpl>() } {
  registerEventHandler( _eventProcessorImpl->defaultQuitEventHandler );
}

EventProcessor::~EventProcessor() {};

void EventProcessor::run() {
  SDL_Event event;  
  while ( not _eventProcessorImpl->quitFlag ) {
    SDL_WaitEvent(&event);
      try {
        for(const auto &eventHandler : _eventProcessorImpl->_eventHandlers) {
            switch (event.type) {
              case SDL_EventType::SDL_MOUSEBUTTONDOWN:
              case SDL_EventType::SDL_MOUSEBUTTONUP:
                createMouseButtonEvent((SDL_MouseButtonEvent*)(&event)).handle(eventHandler);
                break;
              case SDL_EventType::SDL_QUIT:
                createQuitEvent((SDL_QuitEvent*)(&event)).handle(eventHandler);
                break;
            }
        }
      } catch (std::range_error& rangeError) {} // TODO: map all the event types
  }
}

void EventProcessor::registerEventHandler(BaseEventHandler& baseEventHandler ) {
  _eventProcessorImpl->_eventHandlers.push_front(std::ref(baseEventHandler));
}


QuitEvent createQuitEvent([[maybe_unused]] const SDL_QuitEvent* sdlQuitEvent) {
  QuitEvent quitEvent {
    std::chrono::milliseconds( SDL_GetTicks64() )
  };
  return quitEvent;
}

MouseButtonEvent createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent) {
  MouseButtonEvent mouseButtonEvent {
    std::chrono::milliseconds( SDL_GetTicks64() ),
    sdlMouseButtonEvent->windowID,
    sdlMouseButtonEvent->which,
    sdlMouseButtonEvent->x,
    sdlMouseButtonEvent->y,
    sdlMouseButtonEventButtonMap[sdlMouseButtonEvent->button],
    sdlMouseButtonEventStateMap[sdlMouseButtonEvent->state],
    sdlMouseButtonEvent->clicks
  };
  return mouseButtonEvent;
}

}
