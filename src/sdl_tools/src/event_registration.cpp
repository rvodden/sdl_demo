/**
 * @file event_registration.cpp
 * @brief Implementation of EventRegistration RAII token
 */

#include "sdl/event_registration.h"

#include "event_router_impl.h"

namespace sdl::tools {

EventRegistration::EventRegistration()
    : _impl(), _handlerId(0), _registered(false) {}

EventRegistration::EventRegistration(std::weak_ptr<EventRouterImpl> impl,
                                     uint64_t handlerId)
    : _impl(std::move(impl)), _handlerId(handlerId), _registered(true) {}

EventRegistration::EventRegistration(EventRegistration&& other) noexcept
    : _impl(std::move(other._impl)),
      _handlerId(other._handlerId),
      _registered(other._registered) {
  other._registered = false;
  other._handlerId = 0;
}

auto EventRegistration::operator=(EventRegistration&& other) noexcept
    -> EventRegistration& {
  if (this != &other) {
    // Deregister current handler before taking ownership of new one
    unregister();

    _impl = std::move(other._impl);
    _handlerId = other._handlerId;
    _registered = other._registered;

    other._registered = false;
    other._handlerId = 0;
  }
  return *this;
}

EventRegistration::~EventRegistration() { unregister(); }

void EventRegistration::unregister() {
  if (!_registered) {
    return;
  }

  // Try to lock the weak_ptr to get access to the router
  if (auto impl = _impl.lock()) {
    impl->unregisterHandler(_handlerId);
  }

  // Mark as unregistered regardless of whether router still exists
  _registered = false;
}

auto EventRegistration::isRegistered() const -> bool {
  if (!_registered) {
    return false;
  }

  // Check if router still exists
  if (_impl.expired()) {
    return false;
  }

  return true;
}

}  // namespace sdl::tools
