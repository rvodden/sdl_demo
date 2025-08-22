#include <event.h>
#include <event_router.h>
#include <sdl.h>
#include <user_event.h>

#include <chrono>
#include <iostream>
#include <memory>

using namespace sdlpp;
using namespace sdlpp::tools;

class TestEvent : public CustomUserEvent<TestEvent> {
 public:
  TestEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
            int testData)
      : CustomUserEvent(ts, winId), data(testData) {}

  int data;
};

class TestEventHandler : public EventHandler<TestEvent>,
                         public BaseEventHandler {
 public:
  void handle(const TestEvent& testEvent) override {
    std::cout << "TestEvent handler called! Data: " << testEvent.data << "\n";
    handlerCalled = true;
  }

  bool handlerCalled = false;
};

auto main() -> int {
  try {
    SDL sdl;
    sdl.initSubSystem(SDL::kVideo);

    auto eventBus = std::make_shared<EventBus>();
    auto eventRouter = std::make_shared<EventRouter>(eventBus);

    std::cout << "Creating TestEvent handler...\n";
    auto testHandler = std::make_unique<TestEventHandler>();
    auto* handlerPtr = testHandler.get();

    std::cout << "Registering event handler for TestEvent...\n";
    eventRouter->registerEventHandler(*testHandler);

    std::cout << "Publishing TestEvent...\n";
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());

    std::cout << "TestEvent type: " << TestEvent::getEventType() << "\n";
    eventBus->publish(std::make_unique<TestEvent>(now, 0, 42));  // NOLINT(cppcoreguidelines-avoid-magic-numbers)

    std::cout << "Waiting for event...\n";

    // Simple test: just try to get one event
    try {
      auto event = eventBus->wait();
      std::cout << "Got event from producer, now checking if handler was "
                   "called...\n";

      // The event should have been dispatched automatically if our fix
      // works Let's manually dispatch it to see what happens
      std::cout << "Manually dispatching event to handler...\n";
      event->handle(*handlerPtr);

      std::cout << "Handler called: "
                << (handlerPtr->handlerCalled ? "YES" : "NO") << "\n";
    } catch (sdlpp::UnknownEventException& e) {
      std::cout << "Unknown event exception: " << e.what() << "\n";
    }
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << "\n";
    return -1;
  }

  std::cout << "Test completed.\n";
  return 0;
}
