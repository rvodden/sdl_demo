#include <iterator>
#include <functional>
#include <optional>
#include <memory>
#include <typeindex>
#include <utility>
#include <vector>

#include "sdl/event.h"
#include "sdl/event_router.h"

#include "event_router_impl.h"

namespace sdl::tools {

void DefaultQuitEventHandler::handle(
    [[maybe_unused]] const QuitEvent& quitEvent) {
  _eventRouterImpl.get().quit();
}

EventRouter::EventRouter(std::shared_ptr<BaseEventBus> eventBus)
    : _impl(
          std::make_unique<EventRouterImpl>(std::move(eventBus))) {
  registerEventHandler(_impl->defaultQuitEventHandler);
}

EventRouter::~EventRouter() = default;

EventRouter::EventRouter(EventRouter&& other) noexcept = default;

auto EventRouter::operator=(EventRouter&& other) noexcept -> EventRouter& = default;

void EventRouter::run() {
  std::unique_ptr<BaseEvent> event;
  while (!_impl->quitFlag) {
    try {
      event = _impl->_eventBus->wait();
      _impl->dispatchEvent(*event);
    } catch ([[maybe_unused]] sdl::UnknownEventException&) {  // NOLINT(bugprone-empty-catch)
    }
    // TODO: unknown events are "fine" whilst we've not implemented
    // all the events. When we have coverage this catch should go.
  }
}

auto EventRouter::hasEvents() const -> bool {
  // Check if there are events available by doing a non-blocking poll
  // We don't actually consume the event here, just check availability
  auto maybeEvent = _impl->_eventBus->poll();
  return maybeEvent.has_value();
}

auto EventRouter::processNextEvent() -> bool {
  if (_impl->quitFlag) {
    return false;
  }

  try {
    auto maybeEvent = _impl->_eventBus->poll();
    if (maybeEvent.has_value()) {
      _impl->dispatchEvent(*maybeEvent.value());
      return true;
    }
    return false;
  } catch ([[maybe_unused]] sdl::UnknownEventException&) {  // NOLINT(bugprone-empty-catch)
    // Unknown events are currently ignored
    return false;
  }
}

void EventRouter::routeEvent(std::unique_ptr<BaseEvent> event) {
  if (!event) {
    return;
  }
  
  _impl->dispatchEvent(*event);
}

void EventRouter::routeEvent(const BaseEvent& event) {
  _impl->dispatchEvent(const_cast<BaseEvent&>(event));
}

void EventRouter::registerEventHandler(BaseEventHandler& baseEventHandler) {
  _impl->_eventHandlers.push_back(std::ref(baseEventHandler));
}

void EventRouter::registerTypedEventHandlerImpl(std::type_index eventType, sdl::BaseEventHandler& handler) {
  _impl->registerTypedEventHandlerByTypeIndex(eventType, handler);
}

EventRouterImpl::~EventRouterImpl() = default;

EventRouterImpl::EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus)
  : _eventBus(std::move(eventBus)) {};

void EventRouterImpl::quit() { quitFlag = true; };


template<sdl::KeyCode Key>
static void dispatchSpecificKeyForKey(const KeyboardEvent& keyboardEvent, 
                                     const std::vector<std::reference_wrapper<sdl::BaseEventHandler>>& handlers) {
  if (keyboardEvent.keycode != Key) {
    return;
  }
  
  // Dispatch key+direction specific event
  if (keyboardEvent.down) {
    auto specificEvent = SpecificKeyboardEvent<Key, KeyDirection::Down>(
      keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
      keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
      keyboardEvent.isRepeat, keyboardEvent.keymod);
    for (const auto& handler : handlers) {
      specificEvent.handle(handler);
    }
  } else {
    auto specificEvent = SpecificKeyboardEvent<Key, KeyDirection::Up>(
      keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
      keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
      keyboardEvent.isRepeat, keyboardEvent.keymod);
    for (const auto& handler : handlers) {
      specificEvent.handle(handler);
    }
  }
  
  // Also dispatch the key-only specific event (both up and down)
  auto keyOnlyEvent = SpecificKeyboardEvent<Key>(
    keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
    keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
    keyboardEvent.isRepeat, keyboardEvent.keymod);
  for (const auto& handler : handlers) {
    keyOnlyEvent.handle(handler);
  }
}

template<std::size_t... Is>
static void dispatchSpecificKeyboardEventsImpl(const KeyboardEvent& keyboardEvent,
                                              const std::vector<std::reference_wrapper<sdl::BaseEventHandler>>& handlers,
                                              std::index_sequence<Is...>) {
  // Call dispatchSpecificKeyForKey for each KeyCode in the array
  (dispatchSpecificKeyForKey<sdl::kSpecificKeyboardEventSupportedKeys[Is]>(keyboardEvent, handlers), ...);
}

void EventRouterImpl::dispatchEvent(BaseEvent& event) {
  // Use standard polymorphic dispatch for all handlers
  for (const auto& handler : _eventHandlers) {
    event.handle(handler);
  }
  
  // If this is a keyboard event, also create and dispatch specific keyboard events
  if (auto* keyboardEvent = dynamic_cast<KeyboardEvent*>(&event)) {
    dispatchSpecificKeyboardEvents(*keyboardEvent);
  }
}

void EventRouterImpl::dispatchSpecificKeyboardEvents(const KeyboardEvent& keyboardEvent) {
  // Use index_sequence to generate template calls for all supported key codes
  dispatchSpecificKeyboardEventsImpl(keyboardEvent, _eventHandlers, 
                                   std::make_index_sequence<sdl::kSpecificKeyboardEventSupportedKeys.size()>{});
}

}  // namespace sdl::tools
