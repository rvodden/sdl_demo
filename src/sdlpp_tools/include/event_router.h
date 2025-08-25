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
 * @brief Central event distribution system that routes events from event buses to registered handlers
 *
 * EventRouter serves as the core component of the event system, acting as a mediator
 * between event sources (EventBus) and event consumers (handlers). It maintains a
 * registry of event handlers and efficiently distributes incoming events to all
 * handlers capable of processing each specific event type.
 *
 * The router uses the visitor pattern combined with dynamic_cast to achieve type-safe
 * event handling without requiring handlers to know about all possible event types.
 * This allows the system to be easily extended with new event types and handlers
 * without modifying existing code.
 *
 * Key features:
 * - **Type-safe event dispatch**: Handlers only receive events they can process
 * - **Multiple handler support**: Multiple handlers can process the same event
 * - **Lambda integration**: Supports both class-based handlers and lambda functions
 * - **Automatic cleanup**: Handles registration/unregistration automatically
 * - **Thread-safe**: Can be safely used from multiple threads (with appropriate locking)
 *
 * The router supports both blocking (run()) and non-blocking (processNextEvent())
 * operation modes, making it suitable for integration with various application
 * architectures including traditional game loops and callback-based systems.
 *
 * @note EventRouter cannot be copied, only moved for performance reasons
 * @note The event bus must remain valid for the lifetime of the router
 * @note Handler execution is synchronous - avoid blocking operations in handlers
 * @see BaseEventBus for event sources
 * @see EventHandler for type-safe handler interface
 * @see FunctionEventHandler for lambda-based handlers
 *
 * Usage example:
 * @code
 * // Setup event system
 * auto eventBus = std::make_shared<EventBus>();
 * auto eventRouter = std::make_shared<EventRouter>(eventBus);
 * 
 * // Register lambda handler
 * eventRouter->registerEventHandler<MouseButtonEvent>([](const MouseButtonEvent& e) {
 *     if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
 *         std::cout << "Left click at " << e.x << ", " << e.y << std::endl;
 *     }
 * });
 * 
 * // Register class-based handler
 * MyQuitHandler quitHandler;
 * eventRouter->registerEventHandler(quitHandler);
 * 
 * // Run event loop (blocking)
 * eventRouter->run();
 * 
 * // Or process events manually (non-blocking)
 * while (running) {
 *     eventRouter->processNextEvent();
 *     updateGame();
 *     renderFrame();
 * }
 * @endcode
 */
class SDLPP_TOOLS_EXPORT EventRouter {
 public:
  /**
   * @brief Create an event router connected to the specified event bus
   * @param eventBus Shared pointer to the event bus that will supply events (must remain valid)
   * @throws sdlpp::Exception if router initialization fails
   *
   * Creates a new event router that will receive events from the specified event bus
   * and distribute them to registered handlers. The router automatically registers
   * itself with the event bus and sets up the necessary infrastructure for event
   * processing.
   *
   * The event bus must remain valid for the lifetime of this router. The router
   * maintains a shared ownership of the event bus to ensure it remains available.
   *
   * A default quit event handler is automatically registered to handle QuitEvent
   * instances, allowing the run() method to terminate gracefully when requested.
   *
   * @note The event bus must remain valid for the lifetime of this router
   * @note A default quit handler is automatically registered
   * @note Router setup is completed immediately during construction
   */
  EventRouter(std::shared_ptr<BaseEventBus> eventBus);
  
  /**
   * @brief Copy constructor is explicitly deleted
   * @note EventRouters cannot be copied due to complex handler registration state
   */
  EventRouter(const EventRouter& other) = delete;
  
  /**
   * @brief Move constructor - transfers ownership of the router and all registered handlers
   * @param other The router to move from (will be left in valid but unspecified state)
   * @note The moved-from router should not be used except for destruction
   * @note All handler registrations are transferred to the new router
   */
  EventRouter(EventRouter&& other) noexcept;

  /**
   * @brief Copy assignment is explicitly deleted
   * @note EventRouters cannot be copied due to complex handler registration state
   */
  auto operator=(const EventRouter& other) -> EventRouter& = delete;
  
  /**
   * @brief Move assignment operator - transfers ownership of the router and all registered handlers
   * @param other The router to move from (will be left in valid but unspecified state)
   * @return Reference to this router
   * @note The moved-from router should not be used except for destruction
   * @note All handler registrations are transferred to this router
   */
  auto operator=(EventRouter&& other) noexcept -> EventRouter&;

  /**
   * @brief Destructor - automatically stops event processing and cleans up all handlers
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   * @note All registered handlers are automatically unregistered
   * @note Any running event loop is gracefully terminated
   */
  ~EventRouter();

  /**
   * @brief Start the main event processing loop (blocking)
   *
   * Starts an infinite event processing loop that will continue until a quit
   * event is received or an unrecoverable error occurs. The loop follows this
   * pattern:
   * 1. Wait for the next event from the event bus (blocking)
   * 2. Dispatch the event to all registered handlers that can process it
   * 3. Repeat until a QuitEvent is received
   *
   * This is the primary way to run an event-driven application. The method
   * will not return until the application is requested to quit, making it
   * suitable for traditional SDL-style application loops.
   *
   * @note This is a blocking call that will not return until termination
   * @note A QuitEvent will cause the loop to terminate gracefully
   * @note Handler exceptions are caught to prevent loop termination
   * @note Unknown event types are silently ignored (logged in debug builds)
   * @warning Do not call this from multiple threads simultaneously
   *
   * Typical usage pattern:
   * @code
   * // Setup application
   * setupGame();
   * registerAllHandlers();
   * 
   * // Run until quit
   * eventRouter->run();
   * 
   * // Cleanup after quit
   * cleanupGame();
   * @endcode
   */
  void run();

  /**
   * @brief Check if events are available for processing without consuming them
   * @return True if events are available, false otherwise
   *
   * Performs a non-blocking check to determine if events are waiting to be
   * processed. This is useful for integrating with custom event loops or
   * determining when to call processNextEvent().
   *
   * @note This operation does not consume any events
   * @note This is a fast operation suitable for frequent calling
   * @note Result may change between calls as events arrive
   */
  [[nodiscard]] auto hasEvents() const -> bool;

  /**
   * @brief Process a single event without blocking
   * @return True if an event was processed, false if no events were available
   *
   * Processes exactly one event from the event bus without blocking. If no
   * events are available, returns immediately. This is ideal for integration
   * with custom game loops or other event systems that need precise control
   * over event processing timing.
   *
   * The processed event is dispatched to all registered handlers that can
   * handle the event type, following the same routing logic as run().
   *
   * @note Returns immediately if no events are available (non-blocking)
   * @note Handler exceptions are caught to prevent method failure
   * @note Unknown event types are silently ignored
   * @note Each call processes at most one event
   *
   * Custom event loop pattern:
   * @code
   * while (gameRunning) {
   *     // Process all available events
   *     while (eventRouter->processNextEvent()) {
   *         // Events are processed automatically
   *     }
   *     
   *     // Update game logic
   *     updateGame(deltaTime);
   *     
   *     // Render frame
   *     renderGame();
   * }
   * @endcode
   */
  auto processNextEvent() -> bool;

  /**
   * @brief Route a single event to all registered handlers (callback interface)
   * @param event The event to route to all registered handlers
   *
   * Routes the provided event directly to all registered handlers without
   * going through the event bus. This method is designed to be used as a
   * callback from EventBus or TemplatedEventBus, enabling non-blocking,
   * callback-based event processing suitable for integration with SDL's
   * event callback system.
   *
   * The event is dispatched to all handlers capable of processing it,
   * following the same type-safe routing logic used by run() and
   * processNextEvent().
   *
   * @note This bypasses the event bus queue - event is processed immediately
   * @note Handler exceptions are caught to prevent callback failure
   * @note Null events are safely ignored
   * @note This method is thread-safe for concurrent event routing
   *
   * Callback integration pattern:
   * @code
   * // Setup callback-based event processing
   * eventBus->setRouteCallback([&](std::unique_ptr<BaseEvent> event) {
   *     eventRouter->routeEvent(std::move(event));
   * });
   * 
   * // Events are now routed automatically as they arrive
   * @endcode
   */
  void routeEvent(std::unique_ptr<BaseEvent> event);

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
