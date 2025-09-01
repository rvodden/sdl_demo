#ifndef SDL_TOOLS_EVENT_ROUTER_IMPL_H
#define SDL_TOOLS_EVENT_ROUTER_IMPL_H

#include <atomic>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include "sdl/event_router.h"

namespace sdl::tools {

class EventRouter;
class EventRouterImpl;

class DefaultQuitEventHandler : public sdl::EventHandler<QuitEvent>,
                                public sdl::BaseEventHandler {
 public:
  DefaultQuitEventHandler(EventRouterImpl& eventRouterImpl)
      : _eventRouterImpl(eventRouterImpl) {};
  void handle([[maybe_unused]] const QuitEvent& quitEvent) override;

 private:
  std::reference_wrapper<EventRouterImpl> _eventRouterImpl;
};

class EventRouterImpl {
  friend EventRouter;

 public:
  EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus);
  EventRouterImpl(const EventRouterImpl& impl) = delete;
  EventRouterImpl(EventRouterImpl&& impl) noexcept = delete;

  ~EventRouterImpl();

  auto operator=(const EventRouterImpl& impl) -> EventRouterImpl& = delete;
  auto operator=(EventRouterImpl&& impl) noexcept -> EventRouterImpl& = delete;
  void quit();

  // Non-template method for registering by type_index (for future optimization)
  void registerTypedEventHandlerByTypeIndex(std::type_index eventType, sdl::BaseEventHandler& handler) {
    // For now, just store in type-indexed map for future optimization
    // All handlers are still processed through the standard polymorphic dispatch
    _handlersByType[eventType].emplace_back(handler);
  }

  // Main dispatch method - uses standard polymorphic dispatch for safety
  void dispatchEvent(BaseEvent& event);
  
  // Helper method to dispatch specific keyboard events
  void dispatchSpecificKeyboardEvents(const KeyboardEvent& keyboardEvent);

 private:
  std::shared_ptr<BaseEventBus> _eventBus;
  std::vector<std::reference_wrapper<sdl::BaseEventHandler>>
      _eventHandlers;
  
  // Type-indexed handler storage for fast dispatch - no vtable lookups
  std::unordered_map<std::type_index, std::vector<std::reference_wrapper<sdl::BaseEventHandler>>>
      _handlersByType;
  
  std::atomic_bool quitFlag{false};
  DefaultQuitEventHandler defaultQuitEventHandler{*this};
};

}  // namespace sdl::tools

#endif
