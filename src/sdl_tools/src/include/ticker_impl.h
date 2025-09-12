#ifndef SDL_TOOLS_TICKER_IMPL_H
#define SDL_TOOLS_TICKER_IMPL_H

#include <atomic>
#include <chrono>
#include <memory>

#include "sdl/timer.h"
#include "sdl/event_router.h"
#include "sdl/ticker.h"

namespace sdl::tools {

class TickerImpl {
  friend Ticker;

public:
  TickerImpl(std::shared_ptr<sdl::BaseEventBus> eventBus, std::chrono::milliseconds interval);
  ~TickerImpl();

  // Non-copyable, non-movable
  TickerImpl(const TickerImpl&) = delete;
  auto operator=(const TickerImpl&) -> TickerImpl& = delete;
  TickerImpl(TickerImpl&&) = delete;
  auto operator=(TickerImpl&&) -> TickerImpl& = delete;

  auto start() -> void;
  auto stop() -> void;
  [[nodiscard]] auto isRunning() const -> bool;
  [[nodiscard]] auto getInterval() const -> std::chrono::milliseconds;
  [[nodiscard]] auto getTickCount() const -> uint32_t;

private:
  auto timerCallback() -> bool;

  std::shared_ptr<sdl::BaseEventBus> _eventBus;
  std::unique_ptr<sdl::Timer> _timer;
  std::chrono::milliseconds _interval;
  std::atomic<uint32_t> _tickCount;
  std::atomic<bool> _running;
};

}  // namespace sdl::tools

#endif // SDL_TOOLS_TICKER_IMPL_H