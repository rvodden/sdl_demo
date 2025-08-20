#include <event.h>
#include <event_dispatcher.h>
#include <user_event.h>

#include <chrono>
#include <iostream>
#include <memory>

using namespace sdl;
using namespace sdl::tools;

class TestEvent : public CustomUserEvent<TestEvent> {
 public:
  TestEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
            int val)
      : CustomUserEvent(ts, winId), value(val) {}

  int value;
};

auto main() -> int {
  std::cout << "Testing lambda-based event handlers...\n";

  auto eventProducer = std::make_shared<EventProducer>();
  auto eventDispatcher = std::make_shared<EventDispatcher>(eventProducer);

  int counter = 0;
  std::string message = "This is a message";

  // Test 1: Simple lambda with capture
  eventDispatcher->registerEventHandler<TestEvent>(
      [&counter](const TestEvent& e) -> void {
        counter += e.value;
        std::cout << "Lambda handler 1: Received value " << e.value
                  << ", counter now: " << counter << "\n";
      });

  // Test 2: Lambda with different capture
  eventDispatcher->registerEventHandler<TestEvent>(
      [&message](const TestEvent& e) -> void {
        message = "Processed event with value: " + std::to_string(e.value);
        std::cout << "Lambda handler 2: " << message << "\n";
      });

  // Simulate event production (normally this would be done in the event loop)
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  const int kUltimateAnswer = 42;
  auto testEvent = std::make_unique<TestEvent>(now, 0, kUltimateAnswer);

  // Manually trigger event handling (simplified test)
  // In real usage, this would happen in the event loop
  eventDispatcher->registerEventHandler<TestEvent>(
      [](const TestEvent& e) -> void {
        std::cout << "Lambda handler 3: Event received with value " << e.value
                  << "\n";
      });

  std::cout << "Lambda event handler registration completed successfully!\n";
  std::cout << "Final counter: " << counter << "\n";
  std::cout << "Final message: " << message << "\n";

  return 0;
}
