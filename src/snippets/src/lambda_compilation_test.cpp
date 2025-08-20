#include <event.h>
#include <user_event.h>

#include <chrono>
#include <iostream>
#include <memory>

using namespace sdl;

class TestEvent : public CustomUserEvent<TestEvent> {
 public:
  TestEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
            int val)
      : CustomUserEvent(ts, winId), value(val) {}

  int value;
};

auto main() -> int {
  std::cout << "Testing FunctionEventHandler compilation...\n";

  // Test that our FunctionEventHandler template can be instantiated
  auto lambda = [](const TestEvent& e) -> void {
    std::cout << "Event value: " << e.value << "\n";
  };

  // Create handler with the exact lambda type
  auto handler =
      std::make_unique<FunctionEventHandler<TestEvent, decltype(lambda)>>(
          std::move(lambda));

  std::cout << "FunctionEventHandler instantiation successful!\n";

  // Test manual event handling
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  const auto testEvent = TestEvent(now, 0, 42);
  handler->handle(testEvent);

  std::cout << "Manual event handling works!\n";

  return 0;
}