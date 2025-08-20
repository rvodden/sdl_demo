#include <functional>
#include <vector>

#include "legacy_sdk.h"
#include "visitor_pattern_legacy_sdk.h"

class ConcreteCustomEvent : public CustomEvent {
 public:
  ConcreteCustomEvent() = default;
  ConcreteCustomEvent(CustomEventImpl* impl) : CustomEvent(impl) {};
  ConcreteCustomEvent(uint16_t eventNumber, std::string msg)
      : message{std::move(msg)} {
    (*this).customEventNumber = eventNumber;
  };

  void operator()(const BaseEventHandler& abstractHandler) const override {
    castHandler(*this, abstractHandler);
  };

  [[nodiscard]] auto clone() const -> std::unique_ptr<CustomEvent> override {
    auto newCustomEvent =
        std::make_unique<ConcreteCustomEvent>(this->cloneImpl());
    newCustomEvent->customEventNumber = this->customEventNumber;
    newCustomEvent->message = this->message;
    return newCustomEvent;
  };

  std::string message;
};

class UserEventHandler : public EventHandler<UserEvent> {
 public:
  void handle(const UserEvent& userEvent) const override {
    std::cout << "I am a user event with event number: " << userEvent.userNumber
              << "\n";
  };
};

class SystemEventHandler : public EventHandler<SystemEvent> {
 public:
  void handle(const SystemEvent& systemEvent) const override {
    std::cout << "I am handling a system event: " << systemEvent.systemNumber
              << "\n";
  };
};

class ConcreteCustomEventHandler : public EventHandler<ConcreteCustomEvent> {
 public:
  void handle(const ConcreteCustomEvent& customEvent) const override {
    std::cout << customEvent.message << ": " << customEvent.customEventNumber
              << "\n";
  };
};

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
  UserEventHandler userEventHandler;
  SystemEventHandler systemEventHandler;
  ConcreteCustomEventHandler customEventHandler;

  std::vector<std::reference_wrapper<BaseEventHandler>> eventHandlers{
      std::ref<UserEventHandler>(userEventHandler),
      std::ref<SystemEventHandler>(systemEventHandler),
      std::ref<ConcreteCustomEventHandler>(customEventHandler)};

  std::vector<std::unique_ptr<BaseEvent>> newEvents;
  try {
    while (true) {
      newEvents.emplace_back(getEventPtr());
    }
  } catch (NoEventsException& _) {  // NOLINT(bugprone-empty-catch)
  };

  while (std::size(newEvents) > 0) {
    auto& newEvent = *newEvents.back();
    for (const BaseEventHandler& handler : eventHandlers) {
      newEvent(handler);
    }
    newEvents.pop_back();
  }

  const uint8_t kUltimaeAnswer = 42;
  ConcreteCustomEvent customEvent{kUltimaeAnswer, "your mum!"};

  pushEvent(customEvent);

  try {
    while (true) {
      newEvents.emplace_back(getEventPtr());
    }
  } catch (NoEventsException& _) {  // NOLINT(bugprone-empty-catch)
  };

  while (std::size(newEvents) > 0) {
    const auto& newEvent = *newEvents.back();
    for (const BaseEventHandler& handler : eventHandlers) {
      newEvent(handler);
    }
    newEvents.pop_back();
  }
}
