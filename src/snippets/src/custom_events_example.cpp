/**
 * @file custom_events_example.cpp
 * @brief Example demonstrating how to create and handle custom user events
 *
 * This example shows how users can create custom events that inherit from
 * UserEvent and register handlers for them using the event dispatcher system.
 */

#include <event.h>
#include <event_dispatcher.h>
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
              << event.newScore << " points!" << std::endl;
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
              << event.completionTime << " seconds!" << std::endl;
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
              << event.newScore << std::endl;
  }

  void handle(const LevelCompleteEvent& event) override {
    std::cout << "[LOG] Level event: Level " << event.level << " -> "
              << event.completionTime << "s" << std::endl;
  }
};

/**
 * @brief Mock event producer for testing custom events
 */
class MockEventProducer : public EventProducer {
 public:
  MockEventProducer() : eventCount(0) {}

  std::unique_ptr<BaseEvent> wait() override {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());

    // Simulate different events
    switch (eventCount++ % 4) {
      case 0:
        return std::make_unique<ScoreUpdateEvent>(now, 1, 1, 100);
      case 1:
        return std::make_unique<ScoreUpdateEvent>(now, 1, 2, 250);
      case 2:
        return std::make_unique<LevelCompleteEvent>(now, 1, 1, 45.7f);
      case 3:
        return std::make_unique<QuitEvent>(now);
      default:
        return std::make_unique<QuitEvent>(now);
    }
  }

 private:
  int eventCount;
};

/**
 * @brief Demonstrate custom event system usage
 */
int main() {
  std::cout << "Custom Events Example" << std::endl;
  std::cout << "=====================" << std::endl;

  // Show the automatically registered event codes
  std::cout << "Registered SDL event codes:" << std::endl;
  std::cout << "  ScoreUpdateEvent: " << ScoreUpdateEvent::getEventType()
            << std::endl;
  std::cout << "  LevelCompleteEvent: " << LevelCompleteEvent::getEventType()
            << std::endl;
  std::cout << std::endl;

  try {
    // Create mock event producer
    MockEventProducer producer;

    // Create event dispatcher
    EventDispatcher dispatcher(producer);

    // Create and register event handlers
    ScoreUpdateHandler scoreHandler;
    LevelCompleteHandler levelHandler;
    GameEventLogger logger;

    dispatcher.registerEventHandler(scoreHandler);
    dispatcher.registerEventHandler(levelHandler);
    dispatcher.registerEventHandler(logger);

    std::cout << "Starting event loop (will process a few events then quit)..."
              << std::endl;

    // Run the event loop
    dispatcher.run();

    std::cout << "Event loop finished!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
