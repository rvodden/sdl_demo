#ifndef SDL_TOOLS_EVENT_ROUTER_IMPL_H
#define SDL_TOOLS_EVENT_ROUTER_IMPL_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

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
  /** @brief Information about a registered handler */
  struct HandlerInfo {
    std::reference_wrapper<sdl::BaseEventHandler> handler;
    std::type_index eventType;
  };

  EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus);
  EventRouterImpl(const EventRouterImpl& impl) = delete;
  EventRouterImpl(EventRouterImpl&& impl) noexcept = delete;

  ~EventRouterImpl();

  auto operator=(const EventRouterImpl& impl) -> EventRouterImpl& = delete;
  auto operator=(EventRouterImpl&& impl) noexcept -> EventRouterImpl& = delete;
  void quit();

  /**
   * @brief Register a handler and return a unique handler ID
   * @param handler Reference to the handler to register
   * @param eventType Type index of the event this handler processes
   * @return Unique handler ID for later deregistration
   */
  auto registerHandler(sdl::BaseEventHandler& handler, std::type_index eventType) -> uint64_t;

  /**
   * @brief Unregister a handler by its unique ID
   * @param handlerId The ID of the handler to remove
   * @note Safe to call with non-existent IDs (no-op)
   * @note Safe to call multiple times with same ID (no-op after first)
   */
  void unregisterHandler(uint64_t handlerId);

  /**
   * @brief Set callback to be invoked when a handler is unregistered
   * @param callback Function to call with handler ID when unregistering
   * @note Used by EventRouter to clean up function handlers
   */
  void setUnregisterCallback(std::function<void(uint64_t)> callback);

  /**
   * @brief Check if a handler is currently registered
   * @param handlerId The ID to check
   * @return True if handler is registered, false otherwise
   */
  [[nodiscard]] auto isHandlerRegistered(uint64_t handlerId) const -> bool;

  // Main dispatch method - uses standard polymorphic dispatch for safety
  void dispatchEvent(BaseEvent& event);

  // Helper method to dispatch specific keyboard events
  void dispatchSpecificKeyboardEvents(const KeyboardEvent& keyboardEvent);

 private:

  std::shared_ptr<BaseEventBus> _eventBus;

  /** @brief Central registry mapping handler IDs to handler info */
  std::unordered_map<uint64_t, HandlerInfo> _handlerRegistry;

  /** @brief Type-indexed handler storage mapping event types to handler IDs */
  std::unordered_map<std::type_index, std::vector<uint64_t>> _handlersByType;

  /** @brief Atomic counter for generating unique handler IDs */
  std::atomic<uint64_t> _nextHandlerId{1};  // 0 reserved for invalid

  /** @brief Callback invoked when a handler is unregistered (for function handler cleanup) */
  std::function<void(uint64_t)> _unregisterCallback;

  std::atomic_bool quitFlag{false};
  DefaultQuitEventHandler defaultQuitEventHandler{*this};
};

}  // namespace sdl::tools

#endif
