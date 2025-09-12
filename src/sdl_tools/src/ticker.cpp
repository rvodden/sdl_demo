#include "include/ticker_impl.h"
#include "../include/sdl/ticker.h"

namespace sdl::tools {

// TickerImpl implementation
TickerImpl::TickerImpl(std::shared_ptr<sdl::BaseEventBus> eventBus, std::chrono::milliseconds interval)
    : _eventBus(std::move(eventBus)),
      _timer(std::make_unique<sdl::Timer>()),
      _interval(interval),
      _tickCount(0),
      _running(false) {
}

TickerImpl::~TickerImpl() {
  stop();
}

auto TickerImpl::start() -> void {
  if (_running.load()) {
    return; // Already running
  }

  auto callback = [this]() -> bool {
    return timerCallback();
  };

  _timer->start(_interval, callback);
  _running.store(true);
}

auto TickerImpl::stop() -> void {
  if (!_running.load()) {
    return; // Already stopped
  }

  _timer->stop();
  _running.store(false);
}

auto TickerImpl::isRunning() const -> bool {
  return _running.load();
}

auto TickerImpl::getInterval() const -> std::chrono::milliseconds {
  return _interval;
}

auto TickerImpl::getTickCount() const -> uint32_t {
  return _tickCount.load();
}

auto TickerImpl::timerCallback() -> bool {
  if (!_running.load()) {
    return false; // Stop timer
  }

  uint32_t newTickCount = _tickCount.fetch_add(1) + 1;
  
  // Publish tick event through the event bus
  // This is thread-safe as BaseEventBus handles cross-thread event publishing
  auto tickEvent = std::make_unique<TickEvent>(newTickCount);
  _eventBus->publish(std::move(tickEvent));

  return true; // Continue timer
}

// Ticker public interface implementation
Ticker::Ticker(std::shared_ptr<sdl::BaseEventBus> eventBus, std::chrono::milliseconds interval)
    : _impl(std::make_unique<TickerImpl>(std::move(eventBus), interval)) {
}

Ticker::~Ticker() = default;

auto Ticker::start() -> void {
  _impl->start();
}

auto Ticker::stop() -> void {
  _impl->stop();
}

auto Ticker::isRunning() const -> bool {
  return _impl->isRunning();
}

auto Ticker::getInterval() const -> std::chrono::milliseconds {
  return _impl->getInterval();
}

auto Ticker::getTickCount() const -> uint32_t {
  return _impl->getTickCount();
}

} // namespace sdl::tools