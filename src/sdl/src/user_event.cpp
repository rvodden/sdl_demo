#include "exception.h"
#include "user_event_impl.h"

namespace sdl {

UserEvent::UserEvent() : 
  Event( std::chrono::duration<int64_t, std::milli>(SDL_GetTicks64()) ),  
  _userEventImpl{ std::make_unique<UserEventImpl>() } {
    _userEventImpl->_userEvent = this;
  };

UserEvent::UserEvent( const UserEvent& other ) : 
  Event( other.timestamp ), 
  _userEventImpl( std::make_unique<UserEventImpl>(*other._userEventImpl)) {
    _userEventImpl->_userEvent = this;
  };

UserEvent::UserEvent( UserEvent&& other ) : 
  Event( other.timestamp ), 
  _userEventImpl( std::move(other._userEventImpl) ) {
    _userEventImpl->_userEvent = this;
  };

UserEvent::~UserEvent() {};

UserEventImpl::UserEventImpl() {
  if(! _eventType) {
    _eventType = SDL_RegisterEvents(1);
    if(_eventType == -1) throw Exception("SDL_RegisterEvents");
  }
}

std::unique_ptr<SDL_Event> UserEventImpl::_createSDLUserEvent()
{
  return std::unique_ptr<SDL_Event>(new SDL_Event{ .user={
    *_eventType,
    static_cast<uint32_t>(_userEvent->timestamp.count()),
    0,
    0,
    _userEvent,
    nullptr
  }});
}


std::optional<uint32_t> UserEventImpl::_eventType = std::nullopt;
}
