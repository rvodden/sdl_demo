#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Event;

class BaseEventHandler {
 public:
  BaseEventHandler() = default;
  virtual ~BaseEventHandler() = default;
  BaseEventHandler(BaseEventHandler&&) = delete;
  auto operator=(BaseEventHandler&&) -> BaseEventHandler& = delete;
  BaseEventHandler(const BaseEventHandler&) = delete;
  auto operator=(const BaseEventHandler&) -> BaseEventHandler& = delete;
};

template <class EventClass>
class EventHandler {
 public:
  EventHandler() = default;
  virtual ~EventHandler() = default;
  EventHandler(EventHandler&&) = delete;
  auto operator=(EventHandler&&) -> EventHandler& = delete;
  EventHandler(const EventHandler&) = delete;
  auto operator=(const EventHandler&) -> EventHandler& = delete;

  virtual void handle(const EventClass& event) = 0;
};

template <class EventClass>
void castHandler(const EventClass& eventClass,
                 BaseEventHandler& abstractHandler) {
  try {
    auto& eventHandler =
        dynamic_cast<EventHandler<EventClass>&>(abstractHandler);
    eventHandler.handle(eventClass);
  } catch (std::bad_cast& e) {
  }  // NOLINT(bugprone-empty-catch)
}

class Event {
 public:
  Event() = default;
  virtual ~Event() = default;
  Event(Event&& other) = delete;
  auto operator=(Event&& other) -> Event& = default;
  Event(const Event& other) = delete;
  auto operator=(const Event& other) -> Event& = default;

  virtual void handle(BaseEventHandler& abstractHandler) = 0;
  [[nodiscard]] virtual auto getName() const -> std::string { return "Event"; }
  std::string name{"Event"};
};

class MouseEvent : public Event {
 public:
  void handle(BaseEventHandler& abstractHandler) override {
    castHandler(*this, abstractHandler);
  }
  [[nodiscard]] auto getName() const -> std::string override {
    return "MouseEvent";
  }
};

class MouseEventHandler : public BaseEventHandler,
                          public EventHandler<MouseEvent> {
 public:
  void handle([[maybe_unused]] const MouseEvent& mouseEvent) override {
    std::cout << "I am handling a mouse event." << "\n";
  };
};

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
  std::vector<std::unique_ptr<BaseEventHandler>> eventHandlers;
  eventHandlers.push_back(std::make_unique<MouseEventHandler>());

  for (const auto& eventHandler : eventHandlers) {
    MouseEvent().handle(*eventHandler);
  }
}
