#include <SDL3/SDL.h>

#include "sdl/exception.h"
#include "sdl/timer.h"

#include "timer_impl.h"

namespace sdl {

// TimerImpl implementation
TimerImpl::TimerImpl() : _timerId(0), _interval(0), _running(false) {}

TimerImpl::~TimerImpl() {
  stop();
}


auto TimerImpl::start(std::chrono::milliseconds interval, Timer::Callback callback) -> void {
  stop(); // Stop any existing timer
  
  _callback = std::move(callback);
  _interval = interval;
  
  _timerId = SDL_AddTimer(static_cast<Uint32>(interval.count()), timerCallback, this);
  if (_timerId == 0) {
    throw Exception("SDL_AddTimer");
  }
  
  _running = true;
}

auto TimerImpl::stop() -> void {
  if (_running && _timerId != 0) {
    SDL_RemoveTimer(_timerId);
    _timerId = 0;
    _running = false;
  }
}

auto TimerImpl::isRunning() const -> bool {
  return _running;
}

auto TimerImpl::getInterval() const -> std::chrono::milliseconds {
  return _interval;
}

auto TimerImpl::timerCallback(void* param, SDL_TimerID timerId, Uint32 interval) -> Uint32 {
  [[maybe_unused]] auto intervalMs = interval;
  [[maybe_unused]] auto id = timerId;
  
  auto* impl = static_cast<TimerImpl*>(param);
  if (impl == nullptr || !impl->_running) {
    return 0; // Stop timer
  }
  
  try {
    if (impl->_callback && impl->_callback()) {
      // Continue timer with same interval
      return static_cast<Uint32>(impl->_interval.count());
    } else {
      // Stop timer
      impl->_running = false;
      return 0;
    }
  } catch (...) {
    // Stop timer on exception
    impl->_running = false;
    return 0;
  }
}

// Timer public interface implementation
Timer::Timer() : _impl(std::make_unique<TimerImpl>()) {}

Timer::~Timer() = default;


auto Timer::start(std::chrono::milliseconds interval, Callback callback) -> void {
  _impl->start(interval, std::move(callback));
}

auto Timer::stop() -> void {
  _impl->stop();
}

auto Timer::isRunning() const -> bool {
  return _impl->isRunning();
}

auto Timer::getInterval() const -> std::chrono::milliseconds {
  return _impl->getInterval();
}

} // namespace sdl