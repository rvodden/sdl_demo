#include <atomic>
#include <forward_list>
#include <functional>
#include <vector>

#include <SDL3/SDL.h>

#include "constexpr_map.h"
#include "event.h"
#include "user_event.h"

namespace sdlpp {

static constexpr vodden::Map<uint32_t, MouseButtonEvent::Button, 5>
    sdlMouseButtonEventButtonMap{{
        {SDL_BUTTON_LEFT, MouseButtonEvent::Button::kLeft},
        {SDL_BUTTON_MIDDLE, MouseButtonEvent::Button::kMiddle},
        {SDL_BUTTON_RIGHT, MouseButtonEvent::Button::kRight},
        {SDL_BUTTON_X1, MouseButtonEvent::Button::kX1},
        {SDL_BUTTON_X2, MouseButtonEvent::Button::kX2},
    }};

auto createQuitEvent(const SDL_QuitEvent* sdlQuitEvent)
    -> std::unique_ptr<QuitEvent>;
auto createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent)
    -> std::unique_ptr<MouseButtonEvent>;
auto createUserEvent(const SDL_UserEvent* sdlUserEvent)
    -> std::unique_ptr<BaseEvent>;

/**
 * @brief Template specialization of EventAdaptor for SDL_Event
 *
 * This specialization provides the conversion logic from SDL_Event
 * to sdlpp event objects, reusing the existing conversion functions.
 */
template<>
class EventAdaptor<SDL_Event> {
 public:
  /**
   * @brief Convert SDL_Event to sdlpp event
   * @param event The SDL event to convert
   * @return Converted sdlpp event, or nullptr if conversion not supported
   */
  auto convertEvent(const SDL_Event& event) -> std::unique_ptr<BaseEvent> {
    switch (event.type) {
      case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
        return createMouseButtonEvent(&event.button);
      case SDL_EventType::SDL_EVENT_QUIT:
        return createQuitEvent(&event.quit);
      default:
        // Check if this is a user event
        if (event.type >= SDL_EVENT_USER && event.type < SDL_EVENT_LAST) {
          return createUserEvent(&event.user);
        }
        return nullptr;  // Unsupported event type
    }
  }
};

// EventBus pimpl implementation  
class EventBusImpl {
 public:
  EventBusImpl() = default;
  ~EventBusImpl() = default;

  auto processSDLEvent(const SDL_Event& event) -> std::unique_ptr<BaseEvent>;
  
  void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) {
    _routeCallback = std::move(callback);
  }

 private:
  std::function<void(std::unique_ptr<BaseEvent>)> _routeCallback;
};

/**
 * @brief Concrete SDL event bus implementation using both CRTP and virtual inheritance
 * 
 * This class provides the concrete implementation for SDL event handling
 * using the CRTP-based TemplatedEventBus for zero-cost abstraction while
 * also inheriting from BaseEventBus to allow polymorphic usage.
 */
class SDLEventBus : public TemplatedEventBus<SDLEventBus, SDL_Event>, public BaseEventBus {
 public:
  SDLEventBus() = default;

  // Implementation methods for CRTP base
  auto waitImpl() -> std::unique_ptr<BaseEvent>;
  auto pollImpl() -> std::optional<std::unique_ptr<BaseEvent>>;
  void publishImpl(std::unique_ptr<UserEvent> event);

  // BaseEventBus interface implementation (delegates to CRTP methods)
  auto wait() -> std::unique_ptr<BaseEvent> override {
    return TemplatedEventBus<SDLEventBus, SDL_Event>::wait();
  }

  auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override {
    return TemplatedEventBus<SDLEventBus, SDL_Event>::poll();
  }

  void publish(std::unique_ptr<UserEvent> event) override {
    TemplatedEventBus<SDLEventBus, SDL_Event>::publish(std::move(event));
  }

  void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override {
    TemplatedEventBus<SDLEventBus, SDL_Event>::setRouteCallback(std::move(callback));
  }
};

/**
 * @brief Factory function to create an SDL event bus
 * 
 * This function creates an SDLEventBus instance that uses CRTP internally
 * for zero-cost abstraction while providing the BaseEventBus interface.
 * 
 * @return A unique pointer to an SDL event bus as BaseEventBus
 */
auto createSDLEventBus() -> std::unique_ptr<BaseEventBus>;

}  // namespace sdlpp
