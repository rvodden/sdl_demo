/**
 * @file event_router.h
 * @brief Event routing system for routing events to registered handlers
 *
 * This file contains the EventRouter class which provides a centralized
 * event routing system. It receives events from an EventBus and
 * routes them to all registered event handlers using the visitor pattern.
 */

#ifndef SDL_TOOLS_EVENT_ROUTER_H
#define SDL_TOOLS_EVENT_ROUTER_H

#include <memory>
#include <vector>

#include "event.h"
#include "sdlpp_tools_export.h"

namespace sdlpp::tools {

/**
 * @brief Forward declaration of the implementation class (pimpl idiom)
 */
class EventRouterImpl;

/**
 * @brief Central event routing system for routing events to handlers
 *
 * The EventRouter acts as a mediator between event buses and event
 * handlers. It maintains a collection of registered event handlers and
 * routes incoming events to all handlers that can process them. The
 * router uses the visitor pattern to achieve type-safe event handling
 * without requiring handlers to know about all possible event types.
 *
 * Usage example:
 * @code
 * EventBus bus;
 * EventRouter router(bus);
 * MyEventHandler handler;
 * router.registerEventHandler(handler);
 * router.run(); // Starts the event loop
 * @endcode
 */
class SDLPP_TOOLS_EXPORT EventRouter {
 public:
  /**
   * @brief Construct an EventRouter with the given event bus
   *
   * The router will use the provided event bus to receive events
   * that need to be routed to registered handlers.
   *
   * @param eventBus Reference to the event bus that will supply
   * events
   */
  EventRouter(std::shared_ptr<BaseEventBus> eventBus);
  EventRouter(const EventRouter& other) = delete;
  EventRouter(EventRouter&& other) noexcept;

  auto operator=(const EventRouter& other) -> EventRouter& = delete;
  auto operator=(EventRouter&& other) noexcept
      -> EventRouter&;

  /**
   * @brief Destructor
   *
   * Cleans up resources and stops the event loop if it's running.
   */
  ~EventRouter();

  /**
   * @brief Start the main event processing loop
   *
   * This method starts an infinite loop that:
   * 1. Waits for events from the event bus
   * 2. Dispatches each event to all registered handlers
   * 3. Continues until a quit event is received or an error occurs
   *
   * This is a blocking call that will not return until the event loop
   * terminates.
   */
  void run();

  /**
   * @brief Register an event handler to receive dispatched events
   *
   * Adds the given event handler to the list of handlers that will receive
   * events. The handler will receive all events it can handle based on its
   * inheritance from EventHandler<T> template specializations.
   *
   * @param baseEventHandler Reference to the event handler to register.
   *                        The handler must remain valid for the lifetime
   *                        of the dispatcher or until it's unregistered.
   *
   * @note Multiple handlers can be registered, and each event will be
   *       dispatched to all compatible handlers.
   * @note The router does not take ownership of the handler.
   */
  void registerEventHandler(sdlpp::BaseEventHandler& baseEventHandler);

  /**
   * @brief Register a callable object (lambda, function, etc.) as an event
   * handler
   *
   * This template method allows you to register lambdas, function objects,
   * and other callable types as event handlers without needing to create
   * separate handler classes. The callable will be wrapped in a
   * FunctionEventHandler and stored internally by the dispatcher.
   *
   * @tparam EventType The type of event the callable should handle
   * @tparam Callable The type of the callable object (automatically deduced)
   * @param callable The callable object that will handle events of EventType
   *
   * Usage example:
   * @code
   * router.registerEventHandler<ClickEvent>([](const ClickEvent& e) {
   *     std::cout << "Clicked at: " << e.x << ", " << e.y << std::endl;
   * });
   * @endcode
   *
   * @note The router takes ownership of the created handler and manages
   * its lifetime.
   */
  template <typename EventType, typename Callable>
  void registerEventHandler(Callable&& callable) {
    auto handler = std::make_unique<sdlpp::FunctionEventHandler<EventType, Callable>>(
        std::forward<Callable>(callable));
    _functionHandlers.push_back(std::move(handler));
    registerEventHandler(*_functionHandlers.back());
  }

 private:
  /** @brief Pointer to implementation (pimpl idiom) */
  std::unique_ptr<EventRouterImpl> _eventRouterImpl;

  /** @brief Storage for function-based event handlers to manage their
   * lifetime */
  std::vector<std::unique_ptr<sdlpp::BaseEventHandler>> _functionHandlers;
};

}  // namespace sdlpp::tools

#endif
