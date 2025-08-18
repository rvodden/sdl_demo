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
  std::cout << "Testing lambda-based event handler compilation...\n";

  // Just test that the template instantiation works
  // This verifies that our FunctionEventHandler template compiles correctly
  using LambdaType = decltype([](const TestEvent& e) {
    std::cout << "Event value: " << e.value << "\n";
  });

  // Test that our FunctionEventHandler template can be instantiated
  auto lambda = [](const TestEvent& e) {
    std::cout << "Event value: " << e.value << "\n";
  };

  auto handler = std::make_unique<FunctionEventHandler<TestEvent, LambdaType>>(
      std::move(lambda));

  std::cout << "Lambda event handler template compilation successful!\n";
  std::cout << "FunctionEventHandler can be instantiated with lambda types.\n";

  // Test manual event handling
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  const auto testEvent = TestEvent(now, 0, 42);
  handler->handle(testEvent);

  std::cout << "Manual event handling works!\n";

  return 0;
}