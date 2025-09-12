#ifndef SDL_TIMER_H
#define SDL_TIMER_H

#include <chrono>
#include <functional>
#include <memory>

#include "sdl_export.h"

namespace sdl {

class TimerImpl;

/**
 * @brief High-resolution timer for scheduled callbacks and timing operations
 *
 * Timer provides scheduled execution of callbacks at specified intervals.
 * Timers run asynchronously and execute callbacks on SDL's timer thread.
 * They are useful for game ticks, periodic updates, and timed events
 * that need to occur independently of the main rendering loop.
 *
 * Timers use RAII for automatic resource management - the underlying
 * SDL timer is created when started and automatically destroyed when
 * the Timer object goes out of scope or is explicitly stopped.
 *
 * @note Timer callbacks execute on SDL's timer thread, not the main thread
 * @note For thread-safe event posting, use event publishing within callbacks
 * @note Timers cannot be copied or moved due to SDL implementation constraints
 * @see UserEvent for thread-safe communication with the main thread
 *
 * Example usage:
 * @code
 * // Create a repeating 250ms game tick
 * sdl::Timer gameTimer;
 * gameTimer.start(std::chrono::milliseconds(250), [&eventBus]() {
 *   eventBus->publish(std::make_unique<GameTickEvent>());
 *   return true; // Continue repeating
 * });
 * 
 * // One-shot timer
 * sdl::Timer oneShot;
 * oneShot.start(std::chrono::seconds(5), []() {
 *   // This runs once after 5 seconds
 *   return false; // Don't repeat
 * });
 * @endcode
 */
class SDL_EXPORT Timer {
public:
  /**
   * @brief Timer callback function type
   * 
   * Callback should return true to continue repeating, false to stop.
   * Executes on SDL's timer thread - use event publishing for thread-safe
   * communication with the main thread.
   */
  using Callback = std::function<bool()>;

  /**
   * @brief Construct a new Timer object
   * 
   * Timer is created in a stopped state. Use start() to begin timing.
   */
  Timer();
  
  /**
   * @brief Destroy the Timer object
   * 
   * Automatically stops the timer if it is running and cleans up resources.
   */
  ~Timer();

  // Non-copyable, non-movable
  Timer(const Timer&) = delete;
  auto operator=(const Timer&) -> Timer& = delete;
  Timer(Timer&& other) = delete;
  auto operator=(Timer&& other) -> Timer& = delete;

  /**
   * @brief Start the timer with specified interval and callback
   * @param interval Time between callback executions
   * @param callback Function to call when timer fires
   * @throws sdl::Exception if timer creation fails
   * 
   * If timer is already running, it will be stopped and restarted
   * with the new parameters. The callback will first execute after
   * the specified interval has elapsed.
   * 
   * @note The callback executes on SDL's timer thread, not the main thread
   * @note Use event publishing within callbacks for thread-safe communication
   */
  auto start(std::chrono::milliseconds interval, Callback callback) -> void;

  /**
   * @brief Stop the timer
   * 
   * Safe to call even if timer is not running.
   * After stopping, the timer can be restarted with different parameters.
   * Any pending callback execution will be cancelled.
   */
  auto stop() -> void;

  /**
   * @brief Check if timer is currently running
   * @return true if timer is active, false otherwise
   */
  [[nodiscard]] auto isRunning() const -> bool;

  /**
   * @brief Get the current interval of the timer
   * @return The interval between callback executions, or zero if not running
   */
  [[nodiscard]] auto getInterval() const -> std::chrono::milliseconds;

private:
  std::unique_ptr<TimerImpl> _impl;
};

} // namespace sdl

#endif // SDL_TIMER_H