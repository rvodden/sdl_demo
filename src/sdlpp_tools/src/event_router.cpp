#include <memory>

#include "event_router_impl.h"

namespace sdlpp::tools {

void DefaultQuitEventHandler::handle(
    [[maybe_unused]] const QuitEvent& quitEvent) {
  _eventRouterImpl.get().quit();
}

EventRouter::EventRouter(std::shared_ptr<BaseEventBus> eventBus)
    : _eventRouterImpl(
          std::make_unique<EventRouterImpl>(std::move(eventBus))) {
  registerEventHandler(_eventRouterImpl->defaultQuitEventHandler);
}

EventRouter::~EventRouter() = default;

EventRouter::EventRouter(EventRouter&& other) noexcept = default;

auto EventRouter::operator=(EventRouter&& other) noexcept -> EventRouter& = default;

void EventRouter::run() {
  std::unique_ptr<BaseEvent> event;
  while (!_eventRouterImpl->quitFlag) {
    try {
      event = _eventRouterImpl->_eventBus->wait();
      for (const auto& handler : _eventRouterImpl->_eventHandlers) {
        event->handle(handler);
      }
    } catch ([[maybe_unused]] sdlpp::UnknownEventException&) {  // NOLINT(bugprone-empty-catch)
    }
    // TODO: unknown events are "fine" whilst we've not implemented
    // all the events. When we have coverage this catch should go.
  }
}

void EventRouter::registerEventHandler(BaseEventHandler& baseEventHandler) {
  _eventRouterImpl->_eventHandlers.push_back(std::ref(baseEventHandler));
}

EventRouterImpl::~EventRouterImpl() = default;

EventRouterImpl::EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus)
  : _eventBus(std::move(eventBus)) {};

void EventRouterImpl::quit() { quitFlag = true; };

}  // namespace sdlpp::tools
