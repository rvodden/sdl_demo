#ifndef __VISITOR_PATTERN_LEGACY_SDK_H__
#define __VISITOR_PATTERN_LEGACY_SDK_H__

#include <iostream>
#include <memory>
#include <typeinfo>
#include <experimental/propagate_const>

class NoEventsException: public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class UnknownEventException: public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class BaseEventHandler { 
  public:
    virtual ~BaseEventHandler() noexcept {};
};

class EventConverter;
template<class EventClass>
class Event;

template <class EventClass>
class InternalEventHandler {
  public:
    virtual ~InternalEventHandler() noexcept {};
    virtual void handle(const EventClass& event) const = 0; // TODO: this should be pure virtual?
};

class BaseConvertable;
class BaseEvent {
  friend void pushEvent(const BaseEvent& event);
  public:
    explicit BaseEvent(const BaseConvertable* convertable);
    
    BaseEvent(const BaseEvent& other);
    BaseEvent& operator=(const BaseEvent& other);
    BaseEvent(BaseEvent&& other) = default;
    BaseEvent& operator=(BaseEvent&& other) = default;
    
    virtual ~BaseEvent() noexcept {};
    virtual void acceptHandler(const BaseEventHandler& abstractHandler) const = 0;
    virtual BaseEvent& clone() const = 0;

  protected:
    BaseConvertable* createConvertable(const BaseEvent* parent) const;
    void setConvertable(const BaseConvertable* convertable);
    void setConvertableParent();
    
    std::unique_ptr<const BaseConvertable, void(*)(const BaseConvertable*)> _convertable;
};

void pushEvent(const BaseEvent& event);

template<class EventClass>
class Event : public BaseEvent {
  public:
    using BaseEvent::BaseEvent;
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;

    virtual void acceptHandler(const BaseEventHandler& abstractHandler) const {
      try {
        const InternalEventHandler<EventClass> &eventHandler = dynamic_cast<const InternalEventHandler<EventClass>&>(abstractHandler);
        eventHandler.handle(*static_cast<const EventClass*>(this));
      } catch (std::bad_cast &e) { } // bad cast just means this handler can't handle this event
    }
    BaseEvent& clone() const {
      auto newEvent = new EventClass(*static_cast<const EventClass*>(this));
      return *newEvent;
    }
};

class UserEventFactory;
class UserEvent : public Event<UserEvent> {
  friend UserEventFactory;
  public:
    UserEvent();
    uint16_t userNumber;
};

class SystemEventFactory;
class SystemEvent : public Event<SystemEvent> {
  friend SystemEventFactory;
  public:
    SystemEvent();
    uint16_t systemNumber;
};

class ConvertableCustomEvent;
class ConvertableCustomEventFactory {
  public:
    static BaseConvertable* createConvertableCustomEvent(BaseEvent* baseEvent);
};

template <class EventClass>
class CustomEventTemplate: public Event<EventClass> {
  public:
    CustomEventTemplate() : Event<EventClass>(ConvertableCustomEventFactory::createConvertableCustomEvent(this)) {};
    virtual void acceptHandler(const BaseEventHandler& abstractHandler) const {
      Event<EventClass>::acceptHandler(abstractHandler);
    }
    virtual BaseEvent& clone() const {
      return Event<EventClass>::clone();
    };
};

template<class EventClass>
class EventHandler: public BaseEventHandler, public InternalEventHandler<EventClass> {};

BaseEvent& getEvent();

#endif
