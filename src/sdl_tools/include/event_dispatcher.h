/**
 * @file event_dispatcher.h
 * @brief Event dispatching system for routing events to registered handlers
 * 
 * This file contains the EventDispatcher class which provides a centralized
 * event routing system. It receives events from an EventProducer and dispatches
 * them to all registered event handlers using the visitor pattern.
 */

#ifndef SDL_TOOLS_EVENT_DISPATCHER_H
#define SDL_TOOLS_EVENT_DISPATCHER_H

#include <memory>
#include <vector>

#include "event.h"

namespace sdl::tools {

/**
 * @brief Forward declaration of the implementation class (pimpl idiom)
 */
class EventDispatcherImpl;

/**
 * @brief Central event dispatching system for routing events to handlers
 * 
 * The EventDispatcher acts as a mediator between event producers and event handlers.
 * It maintains a collection of registered event handlers and dispatches incoming
 * events to all handlers that can process them. The dispatcher uses the visitor
 * pattern to achieve type-safe event handling without requiring handlers to know
 * about all possible event types.
 * 
 * Usage example:
 * @code
 * EventProducer producer;
 * EventDispatcher dispatcher(producer);
 * MyEventHandler handler;
 * dispatcher.registerEventHandler(handler);
 * dispatcher.run(); // Starts the event loop
 * @endcode
 */
class EventDispatcher {
  public:
    /**
     * @brief Construct an EventDispatcher with the given event producer
     * 
     * The dispatcher will use the provided event producer to receive events
     * that need to be dispatched to registered handlers.
     * 
     * @param eventProducer Reference to the event producer that will supply events
     */
    EventDispatcher(std::shared_ptr<EventProducer> eventProducer);
    EventDispatcher(const EventDispatcher& other) = default;
    EventDispatcher(EventDispatcher&& other) noexcept = default;
   
    auto operator=(const EventDispatcher& other) -> EventDispatcher& = default;
    auto operator=(EventDispatcher&& other) noexcept -> EventDispatcher& = default;


    /**
     * @brief Destructor
     * 
     * Cleans up resources and stops the event loop if it's running.
     */
    ~EventDispatcher();
    
    /**
     * @brief Start the main event processing loop
     * 
     * This method starts an infinite loop that:
     * 1. Waits for events from the event producer
     * 2. Dispatches each event to all registered handlers
     * 3. Continues until a quit event is received or an error occurs
     * 
     * This is a blocking call that will not return until the event loop terminates.
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
     * @note The dispatcher does not take ownership of the handler.
     */
    void registerEventHandler(sdl::BaseEventHandler& baseEventHandler);

    /**
     * @brief Register a callable object (lambda, function, etc.) as an event handler
     * 
     * This template method allows you to register lambdas, function objects, and
     * other callable types as event handlers without needing to create separate
     * handler classes. The callable will be wrapped in a FunctionEventHandler
     * and stored internally by the dispatcher.
     * 
     * @tparam EventType The type of event the callable should handle
     * @tparam Callable The type of the callable object (automatically deduced)
     * @param callable The callable object that will handle events of EventType
     * 
     * Usage example:
     * @code
     * dispatcher.registerEventHandler<ClickEvent>([](const ClickEvent& e) {
     *     std::cout << "Clicked at: " << e.x << ", " << e.y << std::endl;
     * });
     * @endcode
     * 
     * @note The dispatcher takes ownership of the created handler and manages its lifetime.
     */
    template<typename EventType, typename Callable>
    void registerEventHandler(Callable&& callable) {
        auto handler = std::make_unique<FunctionEventHandler<EventType, Callable>>(
            std::forward<Callable>(callable)
        );
        _functionHandlers.push_back(std::move(handler));
        registerEventHandler(*_functionHandlers.back());
    }

  private:
    /** @brief Pointer to implementation (pimpl idiom) */
    std::unique_ptr<EventDispatcherImpl> _eventDispatcherImpl;
    
    /** @brief Storage for function-based event handlers to manage their lifetime */
    std::vector<std::unique_ptr<sdl::BaseEventHandler>> _functionHandlers;
};

}

#endif
