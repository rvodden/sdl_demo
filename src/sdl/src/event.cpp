#include "event.h"

#include <SDL3/SDL.h>
#include <constexpr_map.h>

#include <memory>

#include "event_impl.h"
#include "exception.h"
#include "user_event.h"
#include "user_event_impl.h"

namespace sdl {



auto createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent)
    -> std::unique_ptr<MouseButtonEvent> {
  return std::make_unique<MouseButtonEvent>(
      std::chrono::milliseconds(SDL_GetTicks()), sdlMouseButtonEvent->windowID,
      sdlMouseButtonEvent->which, sdlMouseButtonEvent->x,
      sdlMouseButtonEvent->y,
      kSdlMouseButtonEventButtonMap[sdlMouseButtonEvent->button],
      sdlMouseButtonEvent->down, sdlMouseButtonEvent->clicks);
}

auto createUserEvent(const SDL_UserEvent* sdlUserEvent)
    -> std::unique_ptr<BaseEvent> {
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
      std::chrono::milliseconds(SDL_GetTicks()), sdlUserEvent->windowID,
      sdlUserEvent->code, sdlUserEvent->data2);
  std::unique_ptr<BaseEvent> baseEvent = std::move(userEvent);
  return baseEvent;
}

auto createSDLEventBus() -> std::shared_ptr<BaseEventBus> {
  return static_cast<std::shared_ptr<BaseEventBus>>(std::make_shared<SDLEventBus>());
}

// Implementation of CRTP methods for SDLEventBus
auto SDLEventBus::waitImpl() -> std::unique_ptr<BaseEvent> {
  SDL_Event event;
  SDL_WaitEvent(&event);
  EventAdaptor<SDL_Event> adaptor;
  return adaptor.convertEvent(event);
}

auto SDLEventBus::pollImpl() -> std::optional<std::unique_ptr<BaseEvent>> {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    EventAdaptor<SDL_Event> adaptor;
    if (auto convertedEvent = adaptor.convertEvent(event)) {
      return convertedEvent;
    }
  }
  return std::nullopt;
}

void SDLEventBus::publishImpl(std::unique_ptr<UserEvent> userEvent) {
  auto sdlEvent = userEvent->_impl->_createSDLUserEvent();
  
  // Release ownership of the event - it will be reclaimed when SDL returns it
  userEvent.release();  // NOLINT(bugprone-unused-return-value)
  
  auto result = SDL_PushEvent(sdlEvent.get());
  if (!result) {
    throw Exception(SDL_GetError());
  }
}

void SDLEventBus::injectEvent(const std::any& eventData, std::type_index eventTypeId) {
  // SDLEventBus supports SDL_Event injection
  if (eventTypeId == std::type_index(typeid(SDL_Event))) {
    try {
      const auto& sdlEvent = std::any_cast<const SDL_Event&>(eventData);
      // Use the CRTP handlePlatformEvent for zero-cost abstraction
      this->handlePlatformEvent(sdlEvent);
    } catch (const std::bad_any_cast&) {
      // Bad cast - ignore silently
    }
  }
  // Other event types are silently ignored
}


}  // namespace sdl
