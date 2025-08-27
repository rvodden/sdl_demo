#include <memory>
#include <typeindex>

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

void EventRouterImpl::dispatchEvent(BaseEvent& event) {
  // Use standard polymorphic dispatch for all handlers
  for (const auto& handler : _eventHandlers) {
    event.handle(handler);
  }
}

}  // namespace sdl::tools
