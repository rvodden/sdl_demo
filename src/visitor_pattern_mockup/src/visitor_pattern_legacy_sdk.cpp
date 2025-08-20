#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "visitor_pattern_legacy_sdk_impl.h"

// NOLINTBEGIN This is a demo legacy API, it does all kinds of things with raw
// pointers deliberate
//  so thtat we can test if we can hide that behind a modern SDK

BaseEvent::BaseEvent(BaseEventImpl* impl)
    : _impl{impl, [](BaseEventImpl* i) { delete i; }} {};
UserEvent::UserEvent() : BaseEvent(new UserEventImpl(this)) {};
SystemEvent::SystemEvent() : BaseEvent(new SystemEventImpl(this)) {};
CustomEvent::CustomEvent() : BaseEvent(new CustomEventImpl(this)) {};
CustomEvent::CustomEvent(CustomEventImpl* impl)
    : BaseEvent(static_cast<BaseEventImpl*>(impl)) {};

std::unique_ptr<CustomEvent> CustomEvent::clone() const {
  auto customEventImpl =
      static_cast<CustomEventImpl*>(this->_impl.get())->clone();
  auto newCustomEvent = std::make_unique<CustomEvent>(customEventImpl);
  newCustomEvent->customEventNumber = customEventNumber;
  return newCustomEvent;
}

CustomEventImpl* CustomEvent::cloneImpl() const {
  return dynamic_cast<CustomEventImpl*>(this->_impl.get())->clone();
}

CustomEventImpl* CustomEventImpl::clone() const {
  return new CustomEventImpl(*this);
}

std::unique_ptr<UserEvent> createUserEvent(OLD_Event* event) {
  auto userEvent = std::make_unique<UserEvent>();
  userEvent->userNumber = event->user.userNumber;
  return userEvent;
}

std::unique_ptr<SystemEvent> createSystemEvent(OLD_Event* event) {
  auto systemEvent = std::make_unique<SystemEvent>();
  systemEvent->systemNumber = event->system.systemNumber;
  return systemEvent;
}

std::unique_ptr<CustomEvent> createCustomEvent(OLD_Event* event) {
  auto* customEvent = static_cast<CustomEvent*>(event->custom.payload);
  return customEvent->clone();
}

std::unique_ptr<BaseEvent> createEvent(OLD_Event* oldEvent) {
  switch (oldEvent->eventType) {
    case OLD_USEREVENT:
      return createUserEvent(oldEvent);
    case OLD_SYSTEMEVENT:
      return createSystemEvent(oldEvent);
    case OLD_CUSTOMEVENT:
      return createCustomEvent(oldEvent);
    default:
      throw UnknownEventException("Unknown Event");  // TODO: render type number
  }
}

OLD_Event* EventConverter::convert(const UserEventImpl& userEvent) const {
  OLD_Event* oldEvent = new OLD_Event{
      .user = {OLD_USEREVENT,
               static_cast<const UserEvent&>(userEvent._baseEvent).userNumber}};
  return oldEvent;
}

OLD_Event* EventConverter::convert(const SystemEventImpl& systemEvent) const {
  OLD_Event* oldEvent = new OLD_Event{
      .system = {OLD_SYSTEMEVENT,
                 static_cast<const SystemEvent&>(systemEvent._baseEvent)
                     .systemNumber}};
  return oldEvent;
}

OLD_Event* EventConverter::convert(
    const CustomEventImpl& customEventImpl) const {
  auto& customEvent = static_cast<CustomEvent&>(customEventImpl._baseEvent);
  OLD_Event* oldEvent = new OLD_Event{
      .custom = {OLD_CUSTOMEVENT, customEvent.customEventNumber,
                 static_cast<void*>(customEvent.clone().release())}};
  return oldEvent;
}

std::unique_ptr<BaseEvent> getEventPtr() {
  OLD_Event* oldEvent = waitForOldEvent();
  if (oldEvent == nullptr) {
    throw NoEventsException("There are no more events!");
  }
  auto event = createEvent(oldEvent);
  delete oldEvent;
  return event;
}

BaseEvent& getEvent() {
  auto event = getEventPtr();
  return *event.release();  // WARNING: Caller must delete manually - use
                            // getEventPtr() instead
};

void pushEvent(const BaseEvent& event) {
  EventConverter eventConverter{};
  OLD_Event* oldEvent = event._impl->acceptConverter(eventConverter);
  pushOldEvent(oldEvent);
  delete oldEvent;
};

// NOLINTEND
