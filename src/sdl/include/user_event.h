/**
 * @file user_event.h
 * @brief User-defined event for custom application events
 *
 * This file contains UserEvent which provides a way to send custom events
 * through the SDL event system. Applications can inherit from UserEvent to
 * create type-safe custom events while maintaining compatibility with the
 * event dispatcher system.
 */

#ifndef SDL_USER_EVENT_H
#define SDL_USER_EVENT_H

#include "event.h"
#include "sdl_export.h"

namespace sdl {

class UserEventImpl;
class SDLEventBus;
/**
 * @brief Helper function to register SDL event types
 * @return The registered SDL event type ID
 * @throws Exception if SDL_RegisterEvents fails
 */
SDL_EXPORT auto registerEventType() -> uint32_t;

/**
 * @brief User-defined event for custom application events
 *
 * UserEvent provides a way to send custom events through the SDL event
 * system. Applications can inherit from UserEvent to create type-safe custom
 * events while still maintaining compatibility with the event dispatcher
 * system.
 *
 * There are two ways to use UserEvent:
 *
 * 1. Direct instantiation for simple custom events:
 * @code
 * UserEvent event;
 * // Use the event...
 * @endcode
 *
 * 2. Inheritance for type-safe custom events:
 * @code
 * class MyCustomEvent : public UserEvent {
 * public:
 *     MyCustomEvent(int myData) : UserEvent(), customData(myData) = default
 *
 *     virtual void handle(BaseEventHandler &baseEventHandler) override {
 *         castHandler(*this, baseEventHandler);
 *     }
 *
 *     int customData;
 * };
 * @endcode
 */
class SDL_EXPORT UserEvent : public Event {
  friend EventBus;
  friend class SDLEventBus;

 public:
  /**
   * @brief Construct a default user event
   */
  UserEvent();

  /**
   * @brief Construct a user event with specific parameters
   * @param ts Timestamp when the event occurred
   * @param winId ID of the window associated with this event
   * @param cde User-defined event code for categorizing events
   * @param d1 Generic data pointer
   */
  UserEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
            int32_t cde, void* d1);

  UserEvent(const UserEvent& other);
  UserEvent(UserEvent&& other) noexcept;
  auto operator=(const UserEvent& userEvent) -> UserEvent&;
  auto operator=(UserEvent&& userEvent) noexcept -> UserEvent&;
  ~UserEvent() override;

  /**
   * @brief Handle this user event using the provided handler
   * @param baseEventHandler The handler to process this event
   */
  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(*this, baseEventHandler);
  };

  /**
   * @brief Get the window ID associated with this event
   * @return The window ID
   */
  [[nodiscard]] auto getWindowId() const -> uint32_t { return _windowId; }

  /**
   * @brief Set the window ID associated with this event
   * @param winId The window ID to set
   */
  void setWindowId(uint32_t winId) { _windowId = winId; }

  /**
   * @brief Get the user-defined event code
   * @return The event code
   */
  [[nodiscard]] auto getCode() const -> int32_t { return _code; }

  /**
   * @brief Set the user-defined event code
   * @param cde The event code to set
   */
  void setCode(int32_t cde) { _code = cde; }

  /**
   * @brief Get the generic data pointer
   * @return The data pointer
   */
  [[nodiscard]] auto getData() const -> void* { return _data; }

  /**
   * @brief Set the generic data pointer
   * @param d The data pointer to set
   */
  void setData(void* d) { _data = d; }

  auto static getEventType() -> uint32_t {
    static uint32_t eventCode = registerEventType();  // Once per type
    return eventCode;
  }

 private:
  /** @brief ID of the window associated with this event */
  uint32_t _windowId;
  /** @brief User-defined event code for categorizing different event types */
  int32_t _code;
  /** @brief Generic data pointer for passing custom data */
  void* _data;

  std::unique_ptr<UserEventImpl> _impl;
};

/**
 * @brief Template base class for creating type-safe custom user events
 *
 * This template simplifies creating custom events that inherit from
 * UserEvent. It automatically provides the correct handle() method
 * implementation and registers a unique SDL event code for each derived
 * event type.
 *
 * Each derived event type automatically gets its own unique SDL event code
 * through SDL_RegisterEvents, which is called once per type. This ensures
 * proper integration with SDL's event system and avoids code conflicts.
 *
 * Usage:
 * @code
 * class MyCustomEvent : public CustomUserEvent<MyCustomEvent> {
 * public:
 *     MyCustomEvent(int myData) : CustomUserEvent(), customData(myData) =
 * default
 *
 *     int customData;
 * };
 *
 * // Handler usage:
 * class MyHandler : public EventHandler<MyCustomEvent>, public
 * BaseEventHandler { public: void handle(const MyCustomEvent& event)
 * override {
 *         // Handle the custom event
 *         std::cout << "Custom data: " << event.customData << std::endl;
 *     }
 * };
 * @endcode
 *
 * @tparam DerivedEvent The custom event class that inherits from this
 * template
 */
template <typename DerivedEvent>
class CustomUserEvent : public UserEvent {
  friend DerivedEvent;

 public:
  /**
   * @brief Handle this custom event using the provided handler
   *
   * This method ensures that the event dispatcher can correctly route
   * this custom event to handlers that are designed for the specific
   * derived event type.
   *
   * @param baseEventHandler The handler to process this event
   */
  void handle(BaseEventHandler& baseEventHandler) override {
    castHandler(static_cast<const DerivedEvent&>(*this), baseEventHandler);
  }

  auto static getEventType() -> uint32_t {
    static uint32_t eventType = registerEventType();

    return eventType;
  }

 private:
  /**
   * @brief Construct a custom user event with default parameters
   *
   * The event code is automatically set to the registered SDL event type
   * for this specific derived event class.
   */
  CustomUserEvent() : UserEvent() {}

  /**
   * @brief Construct a custom user event with specific parameters
   * @param ts Timestamp when the event occurred
   * @param winId ID of the window associated with this event
   * @param d1 First generic data pointer
   * @param d2 Second generic data pointer
   *
   * The event code is automatically set to the registered SDL event type
   * for this specific derived event class.
   */
  CustomUserEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
                  int32_t cde = 0, void* d1 = nullptr)
      : UserEvent(ts, winId, cde, d1) {}
};

}  // namespace sdl

#endif
