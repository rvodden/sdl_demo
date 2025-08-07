#include <SDL3/SDL.h>

#include "exception.h"
#include "user_event_impl.h"

namespace sdl {

UserEvent::UserEvent() : 
  Event( std::chrono::duration<int64_t, std::milli>(SDL_GetTicks()) ),
  windowId(0), code(0), data(nullptr),
  _userEventImpl{ std::make_unique<UserEventImpl>() } {
    _userEventImpl->_userEvent = this;
  };

UserEvent::UserEvent(
    std::chrono::duration<int64_t, std::milli> ts,
    uint32_t winId,
    int32_t cde,
    void* d1
) : Event(ts), 
    windowId(winId), code(cde), data(d1),
    _userEventImpl{ std::make_unique<UserEventImpl>() } {
    _userEventImpl->_userEvent = this;
  };

UserEvent::UserEvent( const UserEvent& other ) : 
  Event( other.timestamp ),
  windowId(other.windowId), code(other.code), data(other.data),
  _userEventImpl( std::make_unique<UserEventImpl>(*other._userEventImpl)) {
    _userEventImpl->_userEvent = this;
  };

UserEvent::UserEvent( UserEvent&& other ) : 
  Event( other.timestamp ),
  windowId(other.windowId), code(other.code), data(other.data),
  _userEventImpl( std::move(other._userEventImpl) ) {
    _userEventImpl->_userEvent = this;
  };

UserEvent::~UserEvent() {};

UserEventImpl::UserEventImpl() {};

std::unique_ptr<SDL_Event> UserEventImpl::_createSDLUserEvent()
{
  return std::unique_ptr<SDL_Event>(new SDL_Event{ .user={
    .type= _userEvent->getEventType(),
    .reserved = 0, // this is undocumented...
    .timestamp = static_cast<uint32_t>(_userEvent->timestamp.count()),
    .windowID = _userEvent->windowId,
    .code = _userEvent->code,
    .data1 = _userEvent,
    .data2 = _userEvent->data
  }});
}

uint32_t registerEventType() {
  auto eventType = SDL_RegisterEvents(1);
  if (eventType == 0) throw Exception("SDL_RegisterEvents failed for custom event type");

  return eventType;
}

}
