#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <typeindex>
#include <utility>
#include <vector>

#include "sdl/event.h"
#include "sdl/event_registration.h"
#include "sdl/event_router.h"

#include "event_router_impl.h"

namespace sdl::tools {

void DefaultQuitEventHandler::handle(
    [[maybe_unused]] const QuitEvent& quitEvent) {
  _eventRouterImpl.get().quit();
}

EventRouter::EventRouter(std::shared_ptr<BaseEventBus> eventBus)
    : _impl(
          std::make_shared<EventRouterImpl>(std::move(eventBus))) {
  // Set up callback to clean up function handlers when deregistered
  _impl->setUnregisterCallback([this](uint64_t handlerId) {
    _functionHandlers.erase(handlerId);
  });

  // Register default quit handler - store registration to keep it active for router lifetime
  _defaultQuitHandlerRegistration = registerEventHandler(_impl->defaultQuitEventHandler);
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

auto EventRouter::registerEventHandler(BaseEventHandler& baseEventHandler) -> EventRegistration {
  // Use typeid(BaseEvent) as a placeholder since we don't know the specific type
  // This handler will be called for all events via polymorphic dispatch
  uint64_t handlerId = _impl->registerHandler(baseEventHandler, typeid(BaseEvent));
  return EventRegistration(_impl, handlerId);
}

void EventRouter::registerTypedEventHandlerImpl(std::type_index eventType, sdl::BaseEventHandler& handler) {
  _impl->registerHandler(handler, eventType);
}

auto EventRouter::registerHandlerImpl(sdl::BaseEventHandler& handler, std::type_index eventType) -> uint64_t {
  return _impl->registerHandler(handler, eventType);
}

void EventRouter::storeFunctionHandler(uint64_t handlerId, std::unique_ptr<sdl::BaseEventHandler> handler) {
  _functionHandlers[handlerId] = std::move(handler);
}

EventRouterImpl::~EventRouterImpl() = default;

EventRouterImpl::EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus)
  : _eventBus(std::move(eventBus)) {};

void EventRouterImpl::quit() { quitFlag = true; };

auto EventRouterImpl::registerHandler(sdl::BaseEventHandler& handler, std::type_index eventType) -> uint64_t {
  uint64_t handlerId = _nextHandlerId.fetch_add(1);

  // Store handler info in registry
  _handlerRegistry.emplace(handlerId, HandlerInfo{std::ref(handler), eventType});

  // Add to type-indexed map if not BaseEvent (BaseEvent means polymorphic-only dispatch)
  if (eventType != typeid(BaseEvent)) {
    _handlersByType[eventType].push_back(handlerId);
  }

  return handlerId;
}

void EventRouterImpl::unregisterHandler(uint64_t handlerId) {
  // Find handler in registry
  auto it = _handlerRegistry.find(handlerId);
  if (it == _handlerRegistry.end()) {
    return;  // Already unregistered or never existed
  }

  std::type_index eventType = it->second.eventType;

  // Remove from type-indexed map
  if (eventType != typeid(BaseEvent)) {
    auto typeIt = _handlersByType.find(eventType);
    if (typeIt != _handlersByType.end()) {
      auto& ids = typeIt->second;
      ids.erase(std::remove(ids.begin(), ids.end(), handlerId), ids.end());
      if (ids.empty()) {
        _handlersByType.erase(typeIt);
      }
    }
  }

  // Remove from main registry
  _handlerRegistry.erase(it);

  // Notify callback (for function handler cleanup)
  if (_unregisterCallback) {
    _unregisterCallback(handlerId);
  }
}

void EventRouterImpl::setUnregisterCallback(std::function<void(uint64_t)> callback) {
  _unregisterCallback = std::move(callback);
}

auto EventRouterImpl::isHandlerRegistered(uint64_t handlerId) const -> bool {
  return _handlerRegistry.find(handlerId) != _handlerRegistry.end();
}


template<sdl::KeyCode Key>
static void dispatchSpecificKeyForKey(const KeyboardEvent& keyboardEvent,
                                     const std::unordered_map<uint64_t, EventRouterImpl::HandlerInfo>& handlerRegistry) {
  if (keyboardEvent.keycode != Key) {
    return;
  }

  // Dispatch key+direction specific event
  if (keyboardEvent.down) {
    auto specificEvent = SpecificKeyboardEvent<Key, KeyDirection::Down>(
      keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
      keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
      keyboardEvent.isRepeat, keyboardEvent.keymod);
    for (const auto& [id, info] : handlerRegistry) {
      specificEvent.handle(info.handler);
    }
  } else {
    auto specificEvent = SpecificKeyboardEvent<Key, KeyDirection::Up>(
      keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
      keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
      keyboardEvent.isRepeat, keyboardEvent.keymod);
    for (const auto& [id, info] : handlerRegistry) {
      specificEvent.handle(info.handler);
    }
  }

  // Also dispatch the key-only specific event (both up and down)
  auto keyOnlyEvent = SpecificKeyboardEvent<Key>(
    keyboardEvent.timestamp, keyboardEvent.windowId, keyboardEvent.which,
    keyboardEvent.scancode, keyboardEvent.keycode, keyboardEvent.down,
    keyboardEvent.isRepeat, keyboardEvent.keymod);
  for (const auto& [id, info] : handlerRegistry) {
    keyOnlyEvent.handle(info.handler);
  }
}

template<std::size_t... Is>
static void dispatchSpecificKeyboardEventsImpl(const KeyboardEvent& keyboardEvent,
                                              const std::unordered_map<uint64_t, EventRouterImpl::HandlerInfo>& handlerRegistry,
                                              std::index_sequence<Is...>) {
  // Call dispatchSpecificKeyForKey for each KeyCode in the array
  (dispatchSpecificKeyForKey<sdl::kSpecificKeyboardEventSupportedKeys[Is]>(keyboardEvent, handlerRegistry), ...);
}

void EventRouterImpl::dispatchEvent(BaseEvent& event) {
  // Dispatch to handlers in registration order by iterating through handler IDs
  // We need to collect all handler IDs first to maintain registration order
  std::vector<uint64_t> handlerIds;
  handlerIds.reserve(_handlerRegistry.size());

  // Collect all handler IDs in registration order (handler IDs are sequential)
  for (const auto& [id, info] : _handlerRegistry) {
    handlerIds.push_back(id);
  }

  // Sort by handler ID to ensure registration order (lower IDs = registered earlier)
  std::sort(handlerIds.begin(), handlerIds.end());

  // Dispatch in registration order
  for (uint64_t id : handlerIds) {
    auto it = _handlerRegistry.find(id);
    if (it != _handlerRegistry.end()) {
      event.handle(it->second.handler);
    }
  }

  // If this is a keyboard event, also create and dispatch specific keyboard events
  if (auto* keyboardEvent = dynamic_cast<KeyboardEvent*>(&event)) {
    dispatchSpecificKeyboardEvents(*keyboardEvent);
  }
}

void EventRouterImpl::dispatchSpecificKeyboardEvents(const KeyboardEvent& keyboardEvent) {
  // Use index_sequence to generate template calls for all supported key codes
  dispatchSpecificKeyboardEventsImpl(keyboardEvent, _handlerRegistry,
                                   std::make_index_sequence<sdl::kSpecificKeyboardEventSupportedKeys.size()>{});
}

}  // namespace sdl::tools
