#include <memory>

#include <SDL3/SDL.h>

#include <constexpr_map.h>

#include "event.h"
#include "event_impl.h"
#include "exception.h"
#include "user_event.h"
#include "user_event_impl.h"

namespace sdl {

auto EventProducer::wait() -> std::unique_ptr<BaseEvent> {
  SDL_Event event;
  SDL_WaitEvent(&event);
  switch (event.type) {
    case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
      return createMouseButtonEvent(&event.button);
    case SDL_EventType::SDL_EVENT_QUIT:
      return createQuitEvent(&event.quit);
    case SDL_EventType::SDL_EVENT_USER:
      return createUserEvent(&event.user);
    default:
      throw UnknownEventException("I don't know what this event is!");
  }
}

void EventProducer::produce(std::unique_ptr<UserEvent> userEvent) {
  auto sdlEvent = userEvent->_userEventImpl->_createSDLUserEvent();
  
  // Release ownership of the event - it will be reclaimed when SDL returns it
  userEvent.release(); // NOLINT(bugprone-unused-return-value) 
  
  auto result = SDL_PushEvent(sdlEvent.get());
  if(!result) { throw Exception(SDL_GetError()); }
}

auto createQuitEvent([[maybe_unused]] const SDL_QuitEvent* sdlQuitEvent) -> std::unique_ptr<QuitEvent> {
  return std::make_unique<QuitEvent>( std::chrono::milliseconds( SDL_GetTicks() ) );
}

auto createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent) -> std::unique_ptr<MouseButtonEvent> {
  return std::make_unique<MouseButtonEvent>(
    std::chrono::milliseconds( SDL_GetTicks() ),
    sdlMouseButtonEvent->windowID,
    sdlMouseButtonEvent->which,
    sdlMouseButtonEvent->x,
    sdlMouseButtonEvent->y,
    sdlMouseButtonEventButtonMap[sdlMouseButtonEvent->button],
    sdlMouseButtonEvent->down,
    sdlMouseButtonEvent->clicks
  );
}

auto createUserEvent(const SDL_UserEvent* sdlUserEvent) -> std::unique_ptr<BaseEvent> {
  // If data1 contains the original UserEvent pointer, return it directly
  if (sdlUserEvent->data1 != nullptr) {
    auto* originalEvent = static_cast<UserEvent*>(sdlUserEvent->data1);
    // The original event was managed by a unique_ptr that was released when 
    // we pushed it to SDL. We need to take ownership again.
    // This is safe because SDL doesn't delete the data1 pointer.
    auto userEvent = std::unique_ptr<UserEvent>(originalEvent);
    std::unique_ptr<BaseEvent> baseEvent = std::move(userEvent);
    return baseEvent;
  }
  
  // Fallback: create a generic UserEvent for external SDL user events
  auto userEvent = std::make_unique<UserEvent>(
    std::chrono::milliseconds( SDL_GetTicks() ),
    sdlUserEvent->windowID,
    sdlUserEvent->code,
    sdlUserEvent->data2
  );
  std::unique_ptr<BaseEvent> baseEvent = std::move(userEvent);
  return baseEvent; 
}

}
