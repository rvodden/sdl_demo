#include <memory>

#include <SDL3/SDL.h>

#include <constexpr_map.h>

#include "event.h"
#include "event_impl.h"

namespace sdl {

std::unique_ptr<BaseEvent> EventProducer::wait() {
  SDL_Event event;
  SDL_WaitEvent(&event);
  switch (event.type) {
    case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
      return createMouseButtonEvent(reinterpret_cast<SDL_MouseButtonEvent*>(&event));
    case SDL_EventType::SDL_EVENT_QUIT:
      return createQuitEvent(reinterpret_cast<SDL_QuitEvent*>(&event));
    default:
      throw UnknownEventException("I don't know what this event is!");
  }
}

std::unique_ptr<QuitEvent> createQuitEvent([[maybe_unused]] const SDL_QuitEvent* sdlQuitEvent) {
  return std::make_unique<QuitEvent>( std::chrono::milliseconds( SDL_GetTicks() ) );
}

std::unique_ptr<MouseButtonEvent> createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent) {
  return std::make_unique<MouseButtonEvent>(
    std::chrono::milliseconds( SDL_GetTicks() ),
    sdlMouseButtonEvent->windowID,
    sdlMouseButtonEvent->which,
    sdlMouseButtonEvent->x,
    sdlMouseButtonEvent->y,
    sdlMouseButtonEventButtonMap[sdlMouseButtonEvent->button],
    sdlMouseButtonEventStateMap[sdlMouseButtonEvent->down],
    sdlMouseButtonEvent->clicks
  );
}

}
