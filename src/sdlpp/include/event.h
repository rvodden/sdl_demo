/**
 * @file event.h
 * @brief Event system for SDL wrapper providing type-safe event handling
 *
 * This file contains the core event system classes and interfaces that provide
 * a type-safe, polymorphic event handling system built on top of SDL's event
 * system.
 */

#ifndef SDL_EVENT_H
#define SDL_EVENT_H

#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>

#include "sdlpp_export.h"

namespace sdlpp {

class UserEvent;

/**
 * @brief Exception thrown when an unknown or unsupported event type is
 * encountered
 *
 * This exception is thrown when the event system encounters an event type
 * that it doesn't know how to handle or process.
 */
class SDLPP_EXPORT UnknownEventException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * @brief Base class for all event handlers
 *
 * This is the abstract base class that all event handlers must inherit from.
 * It provides the common interface for the polymorphic event handling system.
 */
class SDLPP_EXPORT BaseEventHandler {
 public:
  BaseEventHandler() = default;
  BaseEventHandler(const BaseEventHandler&) = default;
  BaseEventHandler(BaseEventHandler&&) noexcept = default;

  auto operator=(const BaseEventHandler&) -> BaseEventHandler& = default;
  auto operator=(BaseEventHandler&&) -> BaseEventHandler& = default;
  virtual ~BaseEventHandler() = default;
};

/**
 * @brief Template base class for type-specific event handlers
 *
 * This template class provides a type-safe interface for handling specific
 * event types. Event handlers should inherit from this class and implement
 * the handle() method for their specific event type.
 *
 * @tparam EventClass The specific event type this handler can process
 */
template <class EventClass>
class EventHandler {
 public:
  EventHandler() = default;
  EventHandler(const EventHandler&) = default;
  EventHandler(EventHandler&&) noexcept = default;

  auto operator=(const EventHandler&) -> EventHandler& = default;
  auto operator=(EventHandler&&) -> EventHandler& = default;
  virtual ~EventHandler() = default;
  /**
   * @brief Handle a specific event type
   * @param event The event to handle
   */
  virtual void handle(const EventClass& event) = 0;
};

/**
 * @brief Abstract base class for all events
 *
 * This is the base class for all events in the system. It provides the
 * polymorphic interface that allows events to be handled through the
 * visitor pattern implementation.
 */
class SDLPP_EXPORT BaseEvent {
 public:
  virtual ~BaseEvent() = default;

  BaseEvent() = default;
  BaseEvent(const BaseEvent&) = default;
  BaseEvent(BaseEvent&&) noexcept = default;

  auto operator=(const BaseEvent&) -> BaseEvent& = default;
  auto operator=(BaseEvent&&) -> BaseEvent& = default;

  /**
   * @brief Handle this event using the provided handler
   * @param baseEventHandler The handler to process this event
   */
  virtual void handle(BaseEventHandler& baseEventHandler) = 0;
};

/**
 * @brief Cast and dispatch an event to the appropriate typed handler
 *
 * This template function implements the visitor pattern by attempting to
 * cast the base event handler to the specific event handler type and
 * dispatching the event. If the cast fails, it means the handler doesn't
 * support this event type, and the failure is silently ignored.
 *
 * @tparam EventClass The specific event type being handled
 * @param eventClass The event instance to handle
 * @param baseEventHandler The handler to attempt casting and dispatch to
 */
template <class EventClass>
void castHandler(const EventClass& eventClass,
                 BaseEventHandler& baseEventHandler) {
  try {
    auto& eventHandler =
        dynamic_cast<EventHandler<EventClass>&>(baseEventHandler);
    eventHandler.handle(eventClass);
  } catch ([[maybe_unused]] std::bad_cast& e) {  // NOLINT(bugprone-empty-catch) bad cast just
                                // means this handler can't handle this event
  }
}

/**
 * @brief Wrapper class that adapts callable objects to the EventHandler
 * interface
 *
 * This template class allows lambdas, function objects, and other callable
 * types to be used as event handlers by wrapping them in the EventHandler
 * interface. This eliminates the need to create separate handler classes for
 * simple event handling logic.
 *
 * @tparam EventType The type of event this handler processes
 * @tparam Callable The type of the callable object (lambda, function, etc.)
 */
template <typename EventType, typename Callable>
class FunctionEventHandler : public EventHandler<EventType>,
                             public BaseEventHandler {
 public:
  /**
   * @brief Construct a function event handler with the given callable
   * @param callable The callable object that will handle the events
   */
  explicit FunctionEventHandler(Callable&& callable)
      : _callable(std::forward<Callable>(std::move(callable))) {}

  /**
   * @brief Handle an event by calling the wrapped callable
   * @param event The event to handle
   */
  void handle(const EventType& event) override { _callable(event); }

 private:
  std::decay_t<Callable> _callable;
};

/**
 * @brief Base class for all timed events
 *
 * This class extends BaseEvent with timing information, providing a
 * timestamp for when the event occurred. Most concrete event types inherit
 * from this class.
 */
class SDLPP_EXPORT Event : public BaseEvent {
 public:
  /**
   * @brief Construct an event with a timestamp
   * @param ts The timestamp when this event occurred
   */
  Event(std::chrono::duration<uint64_t, std::milli> ts) : timestamp{ts} {};

  /** @brief Timestamp indicating when this event occurred */
  std::chrono::duration<uint64_t, std::milli> timestamp;

  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };
};

/**
 * @brief Event indicating the application should quit
 *
 * This event is typically generated when the user closes the main window
 * or uses a system-level quit command. Handlers should respond by initiating
 * a clean shutdown of the application.
 */
class SDLPP_EXPORT QuitEvent : public Event {
 public:
  using Event::Event;
  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };
};

/**
 * @brief Base class for all mouse-related events
 *
 * This class provides common data for all mouse events, including which
 * window has mouse focus and which mouse device generated the event.
 */
class SDLPP_EXPORT MouseEvent : public Event {
 public:
  /**
   * @brief Construct a mouse event
   * @param ts Timestamp when the event occurred
   * @param winId ID of the window that has mouse focus
   * @param mouseId ID of the mouse device that generated the event
   */
  MouseEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
             uint32_t mouseId)
      : Event(ts), windowId{winId}, which{mouseId} {};

  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };

  /** @brief ID of the window that has mouse focus, if any */
  uint32_t windowId;

  /** @brief ID of the mouse device that generated this event */
  uint32_t which;
};

/**
 * @brief Base class for mouse events that include position information
 *
 * This class extends MouseEvent with x,y coordinate information for events
 * that occur at a specific position within a window.
 */
class SDLPP_EXPORT MousePositionEvent : public MouseEvent {
 public:
  /**
   * @brief Construct a mouse position event
   * @param ts Timestamp when the event occurred
   * @param winId ID of the window that has mouse focus
   * @param mouseId ID of the mouse device that generated the event
   * @param xPos X coordinate of the mouse position relative to the window
   * @param yPos Y coordinate of the mouse position relative to the window
   */
  MousePositionEvent(std::chrono::duration<int64_t, std::milli> ts,
                     uint32_t winId, uint32_t mouseId, float xPos, float yPos)
      : MouseEvent(ts, winId, mouseId), x{xPos}, y{yPos} {};

  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };

  /** @brief X coordinate of the event relative to the window origin */
  float x;
  /** @brief Y coordinate of the event relative to the window origin */
  float y;
};

/**
 * @brief Event generated when a mouse button is pressed or released
 *
 * This event provides information about mouse button state changes,
 * including which button was affected, whether it was pressed or released,
 * and the click count for detecting multi-clicks.
 */
class SDLPP_EXPORT MouseButtonEvent : public MousePositionEvent {
 public:
  /**
   * @brief Enumeration of supported mouse buttons
   */
  enum class Button : uint8_t {
    kLeft,   /**< Left mouse button */
    kMiddle, /**< Middle mouse button (scroll wheel) */
    kRight,  /**< Right mouse button */
    kX1,     /**< Extra mouse button 1 */
    kX2      /**< Extra mouse button 2 */
  };

  /**
   * @brief Construct a mouse button event
   * @param ts Timestamp when the event occurred
   * @param winId ID of the window that has mouse focus
   * @param mouseId ID of the mouse device that generated the event
   * @param xPos X coordinate of the mouse position
   * @param yPos Y coordinate of the mouse position
   * @param btn The mouse button that changed state
   * @param btnDown True if button was pressed, false if released
   * @param clickCount Number of clicks (1=single, 2=double, etc.)
   */
  MouseButtonEvent(std::chrono::duration<int64_t, std::milli> ts,
                   uint32_t winId, uint32_t mouseId, float xPos, float yPos,
                   Button btn, bool btnDown, uint8_t clickCount)
      : MousePositionEvent(ts, winId, mouseId, xPos, yPos),
        button{btn},
        down{btnDown},
        clicks{clickCount} {};

  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };

  /** @brief The mouse button that changed state */
  Button button;

  /** @brief True if the button was pressed, false if released */
  bool down;

  /** @brief Click count: 1 for single click, 2 for double click, etc. */
  uint8_t clicks;
};

/**
 * @brief Template-based event adaptor for converting platform events to sdlpp events
 *
 * EventAdaptors use template specialization to provide zero-cost conversion
 * from platform-specific event types to sdlpp event objects. Each platform
 * (SDL, etc.) provides a specialized implementation.
 *
 * @tparam PlatformEventType The platform-specific event type (e.g., SDL_Event)
 */
template<typename PlatformEventType>
class EventAdaptor {
 public:
  /**
   * @brief Convert platform event to sdlpp event
   * @param event The platform-specific event to convert
   * @return Converted sdlpp event, or nullptr if conversion not possible
   */
  auto convertEvent(const PlatformEventType& event) -> std::unique_ptr<BaseEvent>;
};

/**
 * @brief Abstract base class for event buses (traditional polymorphic interface)
 *
 * This provides a common interface for event buses when runtime polymorphism
 * is needed. For zero-cost abstraction, use TemplatedEventBus directly.
 */
class SDLPP_EXPORT BaseEventBus {
 public:
  BaseEventBus() = default;
  BaseEventBus(const BaseEventBus&) = default;
  BaseEventBus(BaseEventBus&&) noexcept = default;

  auto operator=(const BaseEventBus&) -> BaseEventBus& = default;
  auto operator=(BaseEventBus&&) noexcept -> BaseEventBus& = default;

  virtual ~BaseEventBus() = default;

  /**
   * @brief Wait for and return the next event
   * @return A unique pointer to the next available event
   */
  virtual auto wait() -> std::unique_ptr<BaseEvent> = 0;

  /**
   * @brief Poll for the next event without blocking
   * @return A unique pointer to the next available event, or std::nullopt if no events available
   */
  virtual auto poll() -> std::optional<std::unique_ptr<BaseEvent>> = 0;

  /**
   * @brief Publish a custom event to the event bus
   * @param event The event to publish
   */
  virtual void publish(std::unique_ptr<UserEvent>) = 0;

  /**
   * @brief Set the callback for routing converted events
   * @param callback Function to call when events are available for routing
   */
  virtual void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) = 0;
};

// Forward declaration for pimpl
class EventBusImpl;

/**
 * @brief CRTP template-based event bus for zero-cost platform-specific event handling
 *
 * This template class uses CRTP to provide a zero-cost abstraction for handling events
 * from different platforms. It uses an EventAdaptor to convert platform
 * events and routes them via a callback mechanism suitable for non-blocking
 * event systems.
 *
 * @tparam Derived The concrete event bus implementation (CRTP pattern)
 * @tparam PlatformEventType The platform-specific event type
 * @tparam AdaptorType The adaptor type for converting events (auto-deduced)
 */
template<typename Derived, typename PlatformEventType, typename AdaptorType = EventAdaptor<PlatformEventType>>
class TemplatedEventBus {
 public:
 private:
 TemplatedEventBus() = default;
public:


  /**
   * @brief Set the callback for routing converted events
   * @param callback Function to call when events are converted and ready for routing
   */
  void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) {
    _routeCallback = std::move(callback);
  }

  /**
   * @brief Handle a platform-specific event
   *
   * This method is typically called by platform event callbacks (e.g., SDL_AppEvent).
   * It converts the platform event using the adaptor and routes it via the callback.
   *
   * @param platformEvent The platform-specific event to handle
   */
  void handlePlatformEvent(const PlatformEventType& platformEvent) {
    if (auto event = _adaptor.convertEvent(platformEvent)) {
      if (_routeCallback) {
        _routeCallback(std::move(event));
      }
    }
  }

  /**
   * @brief Wait for and return the next event (delegates to derived implementation)
   * @return A unique pointer to the next available event
   */
  auto wait() -> std::unique_ptr<BaseEvent> {
    return static_cast<Derived*>(this)->waitImpl();
  }

  /**
   * @brief Poll for the next event without blocking (delegates to derived implementation)
   * @return A unique pointer to the next available event, or std::nullopt if no events available
   */
  auto poll() -> std::optional<std::unique_ptr<BaseEvent>> {
    return static_cast<Derived*>(this)->pollImpl();
  }

  /**
   * @brief Publish a custom event to the event bus (delegates to derived implementation)
   * @param event The event to publish
   */
  void publish(std::unique_ptr<UserEvent> event) {
    static_cast<Derived*>(this)->publishImpl(std::move(event));
  }

 private:
  AdaptorType _adaptor;
  std::function<void(std::unique_ptr<BaseEvent>)> _routeCallback;
friend Derived;
};

/**
 * @brief Default event bus implementation
 *
 * This is the main event bus that interfaces with SDL to convert
 * SDL events into the type-safe event objects used by this system.
 */
class SDLPP_EXPORT EventBus : public BaseEventBus {
 public:
  EventBus();
  EventBus(const EventBus&) = delete;
  EventBus(EventBus&&) noexcept;

  auto operator=(const EventBus&) -> EventBus& = delete;
  auto operator=(EventBus&&) noexcept -> EventBus&;

  ~EventBus() override;

  /**
   * @brief Wait for and return the next SDL event
   * @return A unique pointer to the next available event from SDL
   */
  auto wait() -> std::unique_ptr<BaseEvent> override;

  /**
   * @brief Poll for the next SDL event without blocking
   * @return A unique pointer to the next available event, or std::nullopt if no events available
   */
  auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override;

  /**
   * @brief Publish a custom event to the SDL event stream
   * @param event The event to publish
   */
  void publish(std::unique_ptr<UserEvent> event) override;

  /**
   * @brief Set the callback for routing converted events
   * @param callback Function to call when events are available for routing
   */
  void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override;

 private:
  std::unique_ptr<EventBusImpl> _impl;
};

/**
 * @brief Factory function to create an SDL event bus
 * 
 * This function creates an SDLEventBus instance that uses CRTP internally
 * for zero-cost abstraction while providing the BaseEventBus interface.
 * 
 * @return A shared pointer to an SDL event bus as BaseEventBus
 */
[[nodiscard]] SDLPP_EXPORT auto createSDLEventBus() -> std::shared_ptr<BaseEventBus>;

}  // namespace sdlpp

#endif
