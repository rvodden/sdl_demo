/**
 * @file custom_events_example.cpp
 * @brief Example demonstrating how to create and handle custom user events
 *
 * This example shows how users can create custom events that inherit from
 * UserEvent and register handlers for them using the event dispatcher system.
 */

#include <event.h>
#include <event_router.h>
#include <user_event.h>

#include <chrono>
#include <iostream>
#include <memory>

using namespace sdl;
using namespace sdl::tools;

/**
 * @brief Example custom event for game score updates
 *
 * This demonstrates the recommended pattern for creating custom events:
 * 1. Inherit from CustomUserEvent<YourEventType>
 * 2. Define your custom data members
 * 3. The handle() method is provided automatically
 */
class ScoreUpdateEvent : public CustomUserEvent<ScoreUpdateEvent> {
 public:
  ScoreUpdateEvent(std::chrono::duration<int64_t, std::milli> ts,
                   uint32_t winId, int32_t playerIdParam, int32_t newScoreParam)
      : CustomUserEvent(ts, winId),
        playerId(playerIdParam),
        newScore(newScoreParam) {}

  int32_t playerId;
  int32_t newScore;
};

/**
 * @brief Example custom event for level completion
 */
class LevelCompleteEvent : public CustomUserEvent<LevelCompleteEvent> {
 public:
  LevelCompleteEvent(std::chrono::duration<int64_t, std::milli> ts,
                     uint32_t winId, int32_t levelParam,
                     float completionTimeParam)
      : CustomUserEvent(ts, winId),
        level(levelParam),
        completionTime(completionTimeParam) {}

  int32_t level;
  float completionTime;
};

/**
 * @brief Handler for score update events
 *
 * Demonstrates how to create handlers for custom events:
 * 1. Inherit from EventHandler<YourEventType>
 * 2. Inherit from BaseEventHandler
 * 3. Implement the handle() method
 */
class ScoreUpdateHandler : public EventHandler<ScoreUpdateEvent>,
                           public BaseEventHandler {
 public:
  void handle(const ScoreUpdateEvent& event) override {
    std::cout << "Score Update - Player " << event.playerId << " scored "
              << event.newScore << " points!" << "\n";
  }
};

/**
 * @brief Handler for level completion events
 */
class LevelCompleteHandler : public EventHandler<LevelCompleteEvent>,
                             public BaseEventHandler {
 public:
  void handle(const LevelCompleteEvent& event) override {
    std::cout << "Level Complete - Level " << event.level << " completed in "
              << event.completionTime << " seconds!" << "\n";
  }
};

/**
 * @brief Multi-event handler that handles both custom events
 *
 * Demonstrates how a single handler can handle multiple event types
 * by inheriting from multiple EventHandler<T> specializations.
 */
class GameEventLogger : public EventHandler<ScoreUpdateEvent>,
                        public EventHandler<LevelCompleteEvent>,
                        public BaseEventHandler {
 public:
  void handle(const ScoreUpdateEvent& event) override {
    std::cout << "[LOG] Score event: Player " << event.playerId << " -> "
              << event.newScore << "\n";
  }

  void handle(const LevelCompleteEvent& event) override {
    std::cout << "[LOG] Level event: Level " << event.level << " -> "
              << event.completionTime << "s" << "\n";
  }
};

/**
 * @brief Mock event producer for testing custom events
 */
class MockEventBus : public EventBus {
 public:
  MockEventBus() = default;

  auto wait() -> std::unique_ptr<BaseEvent> override {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());

    // Simulate different events
    switch (eventCount++ % 4) {
      case 0:
        return std::make_unique<ScoreUpdateEvent>(
            now, 1, 1, 100);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      case 1:
        return std::make_unique<ScoreUpdateEvent>(
            now, 1, 2, 250);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      case 2:
        return std::make_unique<LevelCompleteEvent>(
            now, 1, 1, 45.7F);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
      case 3:
      default:
        return std::make_unique<QuitEvent>(now);
    }
  }

 private:
  int eventCount = 0;
};

/**
 * @brief Demonstrate custom event system usage
 */
auto main() -> int {
  std::cout << "Custom Events Example" << "\n";
  std::cout << "=====================" << "\n";

  // Show the automatically registered event codes
  std::cout << "Registered SDL event codes:" << "\n";
  std::cout << "  ScoreUpdateEvent: " << ScoreUpdateEvent::getEventType()
            << "\n";
  std::cout << "  LevelCompleteEvent: " << LevelCompleteEvent::getEventType()
            << "\n";
  std::cout << "\n";

  try {
    // Create mock event producer
    std::shared_ptr<MockEventBus> producer =
        std::make_shared<MockEventBus>();

    // Create event dispatcher
    EventRouter dispatcher(producer);

    // Create and register event handlers
    ScoreUpdateHandler scoreHandler;
    LevelCompleteHandler levelHandler;
    GameEventLogger logger;

    dispatcher.registerEventHandler(scoreHandler);
    dispatcher.registerEventHandler(levelHandler);
    dispatcher.registerEventHandler(logger);

    std::cout
        << "Starting event loop (will process a few events then quit)...\n";

    // Run the event loop
    dispatcher.run();

    std::cout << "Event loop finished!\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }

  return 0;
}
