#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "visitor_pattern_legacy_sdk_impl.h"

//NOLINTBEGIN This is a demo legacy API, it does all kinds of things with raw pointers deliberate
// so thtat we can test if we can hide that behind a modern SDK

BaseEvent::BaseEvent(BaseEventImpl* impl): _impl { impl,  [](BaseEventImpl* i ) { delete i; } } {};
UserEvent::UserEvent(): BaseEvent(new UserEventImpl(this))  {};
SystemEvent::SystemEvent(): BaseEvent(new SystemEventImpl(this)) {};
CustomEvent::CustomEvent(): BaseEvent(new CustomEventImpl(this)) {};
CustomEvent::CustomEvent(CustomEventImpl* impl): BaseEvent ( static_cast<BaseEventImpl*>(impl) ) {};

CustomEvent& CustomEvent::clone() const {
  auto customEventImpl = static_cast<CustomEventImpl*>(this->_impl.get())->clone();
  auto newCustomEvent =  new CustomEvent(
    customEventImpl
  );
  newCustomEvent->customEventNumber = customEventNumber;
  return *newCustomEvent;
}

CustomEventImpl* CustomEvent::cloneImpl() const {
  return dynamic_cast<CustomEventImpl*>(this->_impl.get())->clone();
}

CustomEventImpl* CustomEventImpl::clone() const {
  return new CustomEventImpl( *this );
}

UserEvent& createUserEvent(OLD_Event* event) {
  auto *userEvent = new UserEvent();
  userEvent->userNumber = event->user.userNumber;
  return *userEvent;
}

SystemEvent& createSystemEvent(OLD_Event* event) {
  auto *systemEvent = new SystemEvent();
  systemEvent->systemNumber = event->system.systemNumber;
  return *systemEvent;
}

CustomEvent& createCustomEvent(OLD_Event* event) {
  auto* customEvent = static_cast<CustomEvent*>(event->custom.payload);
  return *customEvent;
}

BaseEvent& createEvent(OLD_Event* oldEvent) {
  switch(oldEvent->eventType) {
    case OLD_USEREVENT:
      return createUserEvent(oldEvent);
    case OLD_SYSTEMEVENT:
      return createSystemEvent(oldEvent);
    case OLD_CUSTOMEVENT:
      return createCustomEvent(oldEvent);
    default:
      throw UnknownEventException("Unknown Event"); //TODO: render type number
  }
}

OLD_Event* EventConverter::convert(const UserEventImpl& userEvent) const {
  OLD_Event* oldEvent = new OLD_Event{ .user={
    OLD_USEREVENT,
    static_cast<const UserEvent&>(userEvent._baseEvent).userNumber
  }};
  return oldEvent;
}

OLD_Event* EventConverter::convert(const SystemEventImpl& systemEvent) const {
  OLD_Event* oldEvent = new OLD_Event{ .system={
    OLD_SYSTEMEVENT,
    static_cast<const SystemEvent&>(systemEvent._baseEvent).systemNumber
  }};
  return oldEvent;
}

OLD_Event* EventConverter::convert(const CustomEventImpl& customEventImpl) const {
  auto& customEvent = static_cast<CustomEvent&>(customEventImpl._baseEvent);
  OLD_Event* oldEvent = new OLD_Event{ .custom={
    OLD_CUSTOMEVENT,
    customEvent.customEventNumber,
    static_cast<void *>(&customEvent.clone())
  }};
  return oldEvent;
}

BaseEvent& getEvent() {
  OLD_Event* oldEvent = waitForOldEvent();
  if (oldEvent == nullptr)
  {
    throw NoEventsException("There are no more events!");
  }
  BaseEvent& event = createEvent(oldEvent);
  delete oldEvent;
  return event;
};

void pushEvent(const BaseEvent& event) {
  EventConverter eventConverter {};
  OLD_Event* oldEvent = event._impl->acceptConverter(eventConverter);
  pushOldEvent(oldEvent);
  delete oldEvent;
};

//NOLINTEND
