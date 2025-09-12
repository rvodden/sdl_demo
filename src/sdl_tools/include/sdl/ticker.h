#ifndef SDL_TOOLS_TICKER_H
#define SDL_TOOLS_TICKER_H

#include <chrono>
#include <functional>
#include <memory>

#include "sdl/timer.h"
#include "sdl/event.h"
#include "sdl/user_event.h"

#include "sdl_tools_export.h"

namespace sdl::tools {

class TickerImpl;

/**
 * @brief Custom user event fired when a ticker interval elapses
 * 
 * TickEvent is automatically published to the event system when a Ticker's
 * configured interval passes. This provides a thread-safe way for timer
 * callbacks running on SDL's timer thread to communicate with the main
 * application thread through the event system.
 * 
 * Applications can register handlers for TickEvent to receive periodic
 * notifications for game logic updates, animation frames, or other
 * time-based operations that need to occur independently of rendering.
 */
class SDL_TOOLS_EXPORT TickEvent : public sdl::CustomUserEvent<TickEvent> {
public:
  /**
   * @brief Create a new tick event with tick count
   * @param tickNum The sequential number of this tick (starts from 1)
   */
  explicit TickEvent(uint32_t tickNum) : tickNumber(tickNum) {}
  
  /** @brief Sequential tick number for this event */
  uint32_t tickNumber;
};

/**
 * @brief High-level game timing component that generates periodic tick events
 *
 * Ticker provides a game-oriented abstraction over SDL timers, designed for
 * common game timing patterns like fixed timestep game loops. Instead of 
 * running callbacks on SDL's timer thread, Ticker publishes TickEvent objects
 * to the event system, allowing game logic to run safely on the main thread.
 *
 * This approach separates timing concerns from game logic and integrates
 * seamlessly with the existing event-driven architecture. Handlers receive
 * TickEvent objects with sequential tick numbers, making it easy to implement
 * game state updates, animation frames, and fixed-timestep physics.
 *
 * Key features:
 * - **Thread-safe event integration**: Timer callbacks publish events instead of running game logic
 * - **Sequential tick numbering**: Track tick progression for debugging and state management  
 * - **RAII resource management**: Automatic cleanup when destroyed
 * - **EventRouter integration**: Works seamlessly with existing event handling patterns
 * - **Pause/resume support**: Control timing without losing tick count
 * - **Configurable intervals**: Support any timing requirements from 60fps to slow turn-based games
 *
 * Common use cases:
 * - **Game loops**: Fixed timestep updates independent of rendering framerate
 * - **Animation timing**: Consistent sprite animation progression
 * - **Turn-based games**: Regular prompts for player actions
 * - **Network games**: Synchronized tick-based state updates
 * - **Physics simulation**: Fixed timestep physics integration
 *
 * @note Ticker cannot be copied or moved due to SDL timer integration constraints
 * @note The EventRouter must remain valid for the lifetime of the Ticker
 * @note Tick events are published asynchronously from SDL's timer thread
 * @see TickEvent for the event data structure
 * @see EventRouter for event handling registration
 * @see sdl::Timer for the underlying timer implementation
 *
 * Example usage:
 * @code
 * // Setup event system
 * auto eventBus = createSDLEventBus(); 
 * auto eventRouter = std::make_shared<EventRouter>(eventBus);
 *
 * // Create ticker for 60 FPS game logic (16.67ms intervals)
 * Ticker gameTicker(eventBus, std::chrono::milliseconds(16));
 *
 * // Register handler for game updates
 * eventRouter->registerEventHandler<TickEvent>([&gameState](const TickEvent& e) {
 *     gameState.update();  // Fixed timestep game logic
 *     if (e.tickNumber % 60 == 0) {
 *         std::cout << "One second elapsed (tick " << e.tickNumber << ")\n";
 *     }
 * });
 *
 * // Start ticking
 * gameTicker.start();
 *
 * // Run event loop - tick events will be processed here
 * eventRouter->run();
 *
 * // Ticker automatically stops when destroyed
 * @endcode
 *
 * Snake game example:
 * @code  
 * // Snake moves every 250ms regardless of rendering framerate
 * Ticker snakeTicker(eventBus, std::chrono::milliseconds(250));
 *
 * eventRouter->registerEventHandler<TickEvent>([&snake](const TickEvent&) {
 *     snake.update();  // Move snake forward one cell
 *     checkCollisions(snake);
 *     updateScore(snake);
 * });
 *
 * snakeTicker.start();
 * @endcode
 *
 * Animation example:
 * @code
 * // Character animation updates every 100ms
 * Ticker animTicker(eventBus, std::chrono::milliseconds(100));
 *
 * eventRouter->registerEventHandler<TickEvent>([&sprite](const TickEvent& e) {
 *     // Cycle through animation frames
 *     uint32_t frameIndex = (e.tickNumber - 1) % sprite.getFrameCount();
 *     sprite.setFrame(frameIndex);
 * });
 *
 * animTicker.start();
 * @endcode
 */
class SDL_TOOLS_EXPORT Ticker {
public:
  /**
   * @brief Create a ticker that publishes events at the specified interval
   * @param eventBus Shared pointer to event bus for publishing TickEvent objects
   * @param interval Time between tick events (e.g., std::chrono::milliseconds(250) for 4 Hz)
   * @throws sdl::Exception if ticker creation fails
   * 
   * Creates a new ticker configured to publish TickEvent objects at regular
   * intervals through the provided event bus. The ticker is initially stopped
   * and must be started explicitly with start().
   * 
   * The interval determines the timing precision for game logic. Common values:
   * - 16ms (~60 FPS) - Smooth action games  
   * - 33ms (~30 FPS) - Standard game timing
   * - 100ms (10 Hz) - Turn-based or slow-paced games
   * - 250ms (4 Hz) - Classic Snake game timing
   * - 1000ms (1 Hz) - Once per second updates
   * 
   * @note The event bus must remain valid for the lifetime of this ticker
   * @note Ticker starts in stopped state - call start() to begin ticking
   * @note Events are published from SDL's timer thread but handled on main thread
   */
  Ticker(std::shared_ptr<sdl::BaseEventBus> eventBus, std::chrono::milliseconds interval);

  /**
   * @brief Destructor - automatically stops ticking and cleans up resources
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   * @note Outstanding tick events may still be processed after destruction
   */
  ~Ticker();

  // Non-copyable, non-movable (like Button)
  Ticker(const Ticker&) = delete;
  auto operator=(const Ticker&) -> Ticker& = delete;
  Ticker(Ticker&&) = delete;
  auto operator=(Ticker&&) -> Ticker& = delete;

  /**
   * @brief Start the ticker - begins publishing TickEvent objects at configured intervals
   * @throws sdl::Exception if timer start fails
   * 
   * Begins the periodic timing cycle, with the first TickEvent published after
   * one full interval has elapsed. Tick numbers start from 1 and increment
   * sequentially with each published event.
   * 
   * If the ticker is already running, this method has no effect (it will not
   * restart or reset the tick counter).
   * 
   * @note First tick event occurs after one interval delay, not immediately
   * @note Tick numbering continues from where it left off if resumed after pause
   * @note Safe to call multiple times - subsequent calls are ignored
   */
  auto start() -> void;

  /**
   * @brief Stop the ticker - ceases publishing TickEvent objects
   * 
   * Immediately stops the timing cycle. No further TickEvent objects will be
   * published until start() is called again. The tick counter is preserved,
   * so resuming will continue from the next sequential number.
   * 
   * @note Outstanding events already published may still be processed
   * @note Tick counter is preserved - resume continues sequence
   * @note Safe to call when already stopped
   */
  auto stop() -> void;

  /**
   * @brief Check if the ticker is currently active
   * @return true if ticker is running and publishing events, false otherwise
   * 
   * Returns the current running state of the ticker. This reflects whether
   * the underlying SDL timer is active and publishing TickEvent objects.
   */
  [[nodiscard]] auto isRunning() const -> bool;

  /**
   * @brief Get the current tick interval
   * @return The time duration between tick events
   * 
   * Returns the configured interval between TickEvent publications. This
   * value is set during construction and cannot be changed during the
   * ticker's lifetime.
   */
  [[nodiscard]] auto getInterval() const -> std::chrono::milliseconds;

  /**
   * @brief Get the number of tick events published so far
   * @return The total count of TickEvent objects published since creation
   * 
   * Returns the total number of tick events that have been published since
   * the ticker was created. This count persists across start/stop cycles
   * and can be used for debugging, state management, or progress tracking.
   * 
   * The next tick event published will have tickNumber = getTickCount() + 1.
   * 
   * @note Count includes all published events, even if stop() was called
   * @note Count is not reset by stop()/start() cycles  
   * @note Thread-safe to call while ticker is running
   */
  [[nodiscard]] auto getTickCount() const -> uint32_t;

private:
  std::unique_ptr<TickerImpl> _impl;
};

}  // namespace sdl::tools

#endif // SDL_TOOLS_TICKER_H