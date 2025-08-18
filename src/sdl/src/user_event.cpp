#include <SDL3/SDL.h>

#include "exception.h"
#include "user_event_impl.h"

namespace sdl {

UserEvent::UserEvent()
    : Event(std::chrono::duration<int64_t, std::milli>(SDL_GetTicks())),
      windowId(0),
      code(0),
      data(nullptr),
      _userEventImpl{std::make_unique<UserEventImpl>(this)} {
  _userEventImpl->_userEvent = this;
};

UserEvent::UserEvent(std::chrono::duration<int64_t, std::milli> ts,
                     uint32_t winId, int32_t cde, void* d1)
    : Event(ts),
      windowId(winId),
      code(cde),
      data(d1),
      _userEventImpl{std::make_unique<UserEventImpl>(this)} {
  _userEventImpl->_userEvent = this;
};

UserEvent::UserEvent(const UserEvent& other)
    : Event(other.timestamp),
      windowId(other.windowId),
      code(other.code),
      data(other.data),
      _userEventImpl(std::make_unique<UserEventImpl>(*other._userEventImpl)) {
  _userEventImpl->_userEvent = this;
};

UserEvent::UserEvent(UserEvent&& other) noexcept
    : Event(other.timestamp),
      windowId(other.windowId),
      code(other.code),
      data(other.data),
      _userEventImpl(std::move(other._userEventImpl)) {
  _userEventImpl->_userEvent = this;
};

auto UserEvent::operator=(const UserEvent& userEvent) -> UserEvent& {
  if (this == &userEvent) {
    return *this;
  }

  Event::operator=(userEvent);
  windowId = userEvent.windowId;
  code = userEvent.code;
  data = userEvent.data;
  _userEventImpl = std::make_unique<UserEventImpl>(*userEvent._userEventImpl);
  _userEventImpl->_userEvent = this;

  return *this;
}

auto UserEvent::operator=(UserEvent&& userEvent) noexcept -> UserEvent& {
  if (this == &userEvent) {
    return *this;
  }

  windowId = userEvent.windowId;
  code = userEvent.code;
  data = userEvent.data;
  _userEventImpl = std::move(userEvent._userEventImpl);
  _userEventImpl->_userEvent = this;
  Event::operator=(std::move(userEvent));

  return *this;
}

UserEvent::~UserEvent() = default;

UserEventImpl::UserEventImpl(UserEvent* userEvent) : _userEvent(userEvent) {};

auto UserEventImpl::_createSDLUserEvent() -> std::unique_ptr<SDL_Event> {
  auto retval = std::make_unique<SDL_Event>();

  retval->type =
      _userEvent
          ->getEventType();  // NOLINT(readability-static-accessed-through-instance)
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
