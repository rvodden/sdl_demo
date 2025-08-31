#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace pong {

class Timer {
 public:
  Timer() : _startTime(std::chrono::high_resolution_clock::now()) {};

  auto elapsed() -> float {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - _startTime).count();
    _startTime = currentTime;
    return dt;
  }

  auto reset() -> void {
    _startTime = std::chrono::high_resolution_clock::now();
  }

 private:
  std::chrono::high_resolution_clock::time_point _startTime;
};

} // namespace pong

#endif // TIMER_H
