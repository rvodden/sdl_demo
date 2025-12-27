#ifndef SDL_TIMER_IMPL_H
#define SDL_TIMER_IMPL_H

#include <SDL3/SDL.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>

#include "sdl/timer.h"

namespace sdl {

class TimerImpl {
  friend Timer;

public:
  TimerImpl();
  ~TimerImpl();

  // Non-copyable, non-movable
  TimerImpl(const TimerImpl&) = delete;
  auto operator=(const TimerImpl&) -> TimerImpl& = delete;
  TimerImpl(TimerImpl&& other) = delete;
  auto operator=(TimerImpl&& other) -> TimerImpl& = delete;

  auto start(std::chrono::milliseconds interval, Timer::Callback callback) -> void;
  auto stop() -> void;
  [[nodiscard]] auto isRunning() const -> bool;
  [[nodiscard]] auto getInterval() const -> std::chrono::milliseconds;

private:
  static auto timerCallback(void* param, SDL_TimerID timerId, Uint32 interval) -> Uint32;

  SDL_TimerID _timerId;
  Timer::Callback _callback;
  std::chrono::milliseconds _interval;
  std::atomic<bool> _running;
  std::mutex _callbackMutex;
};

}  // namespace sdl

#endif // SDL_TIMER_IMPL_H