#include <SDL3/SDL.h>

#include "sdl/exception.h"

#include "user_event_impl.h"

namespace sdl {

UserEvent::UserEvent()
    : Event(std::chrono::duration<int64_t, std::milli>(SDL_GetTicks())),
      _windowId(0),
      _code(0),
      _data(nullptr),
      _impl{std::make_unique<UserEventImpl>(this)} {
  _impl->_userEvent = this;
};

UserEvent::UserEvent(std::chrono::duration<int64_t, std::milli> ts,
                     uint32_t winId, int32_t cde, void* d1)
    : Event(ts),
      _windowId(winId),
      _code(cde),
      _data(d1),
      _impl{std::make_unique<UserEventImpl>(this)} {
  _impl->_userEvent = this;
};

UserEvent::UserEvent(const UserEvent& other)
    : Event(other.timestamp),
      _windowId(other._windowId),
      _code(other._code),
      _data(other._data),
      _impl(std::make_unique<UserEventImpl>(*other._impl)) {
  _impl->_userEvent = this;
};

UserEvent::UserEvent(UserEvent&& other) noexcept
    : Event(other.timestamp),
      _windowId(other._windowId),
      _code(other._code),
      _data(other._data),
      _impl(std::move(other._impl)) {
  _impl->_userEvent = this;
};

auto UserEvent::operator=(const UserEvent& userEvent) -> UserEvent& {
  if (this == &userEvent) {
    return *this;
  }

  Event::operator=(userEvent);
  _windowId = userEvent._windowId;
  _code = userEvent._code;
  _data = userEvent._data;
  _impl = std::make_unique<UserEventImpl>(*userEvent._impl);
  _impl->_userEvent = this;

  return *this;
}

auto UserEvent::operator=(UserEvent&& userEvent) noexcept -> UserEvent& {
  if (this == &userEvent) {
    return *this;
  }

  _windowId = userEvent._windowId;
  _code = userEvent._code;
  _data = userEvent._data;
  _impl = std::move(userEvent._impl);
  _impl->_userEvent = this;
  Event::operator=(std::move(userEvent));

  return *this;
}

UserEvent::~UserEvent() = default;

UserEventImpl::UserEventImpl(UserEvent* userEvent) : _userEvent(userEvent) {};

auto UserEventImpl::_createSDLUserEvent() -> std::unique_ptr<SDL_Event> {
  auto retval = std::make_unique<SDL_Event>();

  retval->type =
      _userEvent  // NOLINT(readability-static-accessed-through-instance)
          ->getEventType();
  retval->user.timestamp = static_cast<uint32_t>(_userEvent->timestamp.count());
  retval->user.windowID = _userEvent->getWindowId();
  retval->user.code = _userEvent->getCode();
  retval->user.data1 = _userEvent;
  retval->user.data2 = _userEvent->getData();

  return retval;
}

auto registerEventType() -> uint32_t {
  auto eventType = SDL_RegisterEvents(1);
  if (eventType == 0) {
    throw Exception("SDL_RegisterEvents failed for custom event type");
  }

  return eventType;
}

}  // namespace sdl
