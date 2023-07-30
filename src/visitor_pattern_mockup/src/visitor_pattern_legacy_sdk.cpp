#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "visitor_pattern_legacy_sdk_impl.h"
#include "visitor_pattern_legacy_sdk.h"

BaseEvent::BaseEvent(const BaseConvertable* impl):
  _convertable { impl, [](const BaseConvertable* i ) { delete i; } } 
{ }

BaseEvent::BaseEvent(const BaseEvent& other) : 
  _convertable( 
      std::unique_ptr<const BaseConvertable, void(*)(const BaseConvertable*)>( other.createConvertable(this), [](const BaseConvertable* i){ delete i; } ) 
  ) {} ;

BaseEvent& BaseEvent::operator=(const BaseEvent& other) {
  this->setConvertable(other.createConvertable(this));
  return *this;
}

BaseConvertable::~BaseConvertable() {}

void BaseEvent::setConvertable(const BaseConvertable* convertable) {
  this->_convertable = std::move(
    std::unique_ptr<const BaseConvertable, void(*)(const BaseConvertable*)>(convertable, [](const BaseConvertable* i){ delete i; })
  );
}

void BaseEvent::setConvertableParent() {
  this->_convertable->setParent(this);
}

BaseConvertable* BaseEvent::createConvertable(const BaseEvent* parent) const {
  return this->_convertable->clone(parent);
}

void BaseConvertable::setParent(BaseEvent *baseEvent) const {
  this->_baseEvent = baseEvent;
};

UserEvent::UserEvent(): Event(new ConvertableUserEvent(this)) {};
SystemEvent::SystemEvent(): Event(new ConvertableSystemEvent(this)) {};


UserEvent& UserEventFactory::createUserEvent(OLD_Event* event) {
  UserEvent *userEvent = new UserEvent();
  userEvent->setConvertable(userEvent->createConvertable(userEvent));
  userEvent->userNumber = event->user.userNumber;
  return *userEvent;
}

SystemEvent& SystemEventFactory::createSystemEvent(OLD_Event* event) {
  SystemEvent *systemEvent = new SystemEvent();
  systemEvent->setConvertable(systemEvent->createConvertable(systemEvent));
  systemEvent->systemNumber = event->system.systemNumber;
  return *systemEvent;
}

BaseEvent& CustomEventFactory::createCustomEvent(OLD_Event* event) {
  auto* customEvent = static_cast<BaseEvent*>(event->custom.payload);
  return *customEvent;
}

BaseConvertable* ConvertableCustomEventFactory::createConvertableCustomEvent(BaseEvent* parent) {
  return new ConvertableCustomEvent(parent);
}

BaseEvent& createEvent(OLD_Event* oldEvent) {
  switch(oldEvent->eventType) {
    case OLD_USEREVENT:
      return UserEventFactory::createUserEvent(oldEvent);
    case OLD_SYSTEMEVENT:
      return SystemEventFactory::createSystemEvent(oldEvent);
    case OLD_CUSTOMEVENT:
      return CustomEventFactory::createCustomEvent(oldEvent);
    default:
      throw UnknownEventException("Unknown Event"); //TODO: render type number
  }
}

OLD_Event* EventConverter::convert(const ConvertableUserEvent& userEvent) const {
  OLD_Event* oldEvent = new OLD_Event{ .user={
    OLD_USEREVENT,
    static_cast<const UserEvent*>(userEvent._baseEvent)->userNumber
  }};
  return oldEvent;
}

OLD_Event* EventConverter::convert(const ConvertableSystemEvent& systemEvent) const {
  OLD_Event* oldEvent = new OLD_Event{ .system={
    OLD_SYSTEMEVENT,
    static_cast<const SystemEvent*>(systemEvent._baseEvent)->systemNumber
  }};
  return oldEvent;
}

OLD_Event* EventConverter::convert(const ConvertableCustomEvent& customEventImpl) const {
  const auto& customEvent = static_cast<const BaseEvent&>(*customEventImpl._baseEvent);
  OLD_Event* oldEvent = new OLD_Event{ .custom={
    OLD_CUSTOMEVENT,
    &customEvent.clone()
  }};
  return oldEvent;
}

BaseEvent& getEvent() {
  OLD_Event* oldEvent = waitForOldEvent();
  if (oldEvent == nullptr) throw NoEventsException("There are no more events!");
  BaseEvent& event = createEvent(oldEvent);
  delete oldEvent;
  return event;
};

void pushEvent(const BaseEvent& event) {
  EventConverter eventConverter {};
  OLD_Event* oldEvent = event._convertable->acceptConverter(eventConverter);
  if(oldEvent == nullptr) throw UnknownEventException("I don't know what this event is!");
  pushOldEvent(oldEvent);
  delete oldEvent;
};
