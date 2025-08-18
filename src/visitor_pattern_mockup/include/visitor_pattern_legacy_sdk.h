#ifndef VISITOR_PATTERN_LEGACY_SDK_H
#define VISITOR_PATTERN_LEGACY_SDK_H

#include <iostream>
#include <memory>
#include <typeinfo>

// NOLINTBEGIN This is a demo legacy API, it does all kinds of things with raw
// pointers deliberate
//  so thtat we can test if we can hide that behind a modern SDK

class NoEventsException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class UnknownEventException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class BaseEventHandler {
 public:
  BaseEventHandler() = default;
  BaseEventHandler(const BaseEventHandler& baseEventHandler) = default;
  BaseEventHandler(BaseEventHandler&& baseEventHandler) noexcept = default;

  auto operator=(const BaseEventHandler& baseEventHandler)
      -> BaseEventHandler& = default;
  auto operator=(BaseEventHandler&& baseEventHandler) noexcept
      -> BaseEventHandler& = default;

  virtual ~BaseEventHandler() noexcept = default;
};

template <class EventClass>
class InternalEventHandler {
 public:
  InternalEventHandler() = default;
  InternalEventHandler(const InternalEventHandler& baseEventHandler) = default;
  InternalEventHandler(InternalEventHandler&& baseEventHandler) noexcept =
      default;

  auto operator=(const InternalEventHandler& baseEventHandler)
      -> InternalEventHandler& = default;
  auto operator=(InternalEventHandler&& baseEventHandler) noexcept
      -> InternalEventHandler& = default;

  virtual ~InternalEventHandler() noexcept = default;

  virtual void handle(
      __attribute_maybe_unused__ const EventClass& event) const {};
};

template <class EventClass>
void castHandler(const EventClass& eventClass,
                 const BaseEventHandler& abstractHandler) {
  try {
    auto& eventHandler =
        dynamic_cast<const InternalEventHandler<EventClass>&>(abstractHandler);
    eventHandler.handle(eventClass);
  } catch (std::bad_cast& e) {
  }  // NOLINT(bugprone-empty-catch) bad cast just means this handler can't
     // handle this event
}

class BaseEventImpl;
class BaseEvent {
  friend BaseEventImpl;
  friend void pushEvent(const BaseEvent& event);

 public:
  explicit BaseEvent(BaseEventImpl* impl);
  BaseEvent(const BaseEvent& baseEvent) = default;
  BaseEvent(BaseEvent&& baseEvent) noexcept = default;

  auto operator=(const BaseEvent& baseEvent) -> BaseEvent& = default;
  auto operator=(BaseEvent&& baseEvent) noexcept -> BaseEvent& = default;
  virtual ~BaseEvent() noexcept = default;
  virtual void operator()(const BaseEventHandler& abstractHandler) const = 0;

 protected:
  std::unique_ptr<BaseEventImpl, void (*)(BaseEventImpl*)> _impl;
};

void pushEvent(const BaseEvent& event);

class UserEvent : public BaseEvent {
 public:
  UserEvent();
  uint16_t userNumber;
  void operator()(const BaseEventHandler& abstractHandler) const override {
    castHandler(*this, abstractHandler);
  }
};

class SystemEvent : public BaseEvent {
 public:
  SystemEvent();
  uint16_t systemNumber;
  void operator()(const BaseEventHandler& abstractHandler) const override {
    castHandler(*this, abstractHandler);
  };
};

class CustomEventImpl;
class CustomEvent : public BaseEvent {
  friend CustomEventImpl;

 public:
  CustomEvent();
  CustomEvent(CustomEventImpl* impl);
  CustomEvent(const CustomEvent& baseEvent) = default;
  CustomEvent(CustomEvent&& baseEvent) noexcept = default;

  auto operator=(const CustomEvent& baseEvent) -> CustomEvent& = default;
  auto operator=(CustomEvent&& baseEvent) noexcept -> CustomEvent& = default;
  ~CustomEvent() override = default;

  uint16_t customEventNumber;
  void* payload;
  void operator()(const BaseEventHandler& abstractHandler) const override {
    castHandler(*this, abstractHandler);
  };

  [[nodiscard]] virtual auto clone() const -> std::unique_ptr<CustomEvent>;
  [[nodiscard]] virtual auto cloneImpl() const -> CustomEventImpl*;
};

template <class EventClass>
class EventHandler : public BaseEventHandler,
                     public InternalEventHandler<EventClass> {};

[[deprecated("Use getEventPtr() instead for proper memory management")]]
auto getEvent() -> BaseEvent&;
auto getEventPtr() -> std::unique_ptr<BaseEvent>;

// NOLINTEND

#endif
