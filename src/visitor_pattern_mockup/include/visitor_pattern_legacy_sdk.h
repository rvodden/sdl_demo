#ifndef __VISITOR_PATTERN_LEGACY_SDK_H__
#define __VISITOR_PATTERN_LEGACY_SDK_H__

#include <iostream>
#include <memory>
#include <typeinfo>

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

template <class EventClass>
class InternalEventHandler {
  public:
    virtual ~InternalEventHandler() noexcept {};
    virtual void handle(const EventClass& event) const = 0;
};

template <class EventClass>
void castHandler(const EventClass& eventClass, const BaseEventHandler& abstractHandler) {
  try {
    const InternalEventHandler<EventClass> &eventHandler = dynamic_cast<const InternalEventHandler<EventClass>&>(abstractHandler);
    eventHandler.handle(eventClass);
  } catch (std::bad_cast &e) { } // bad cast just means this handler can't handle this event
}

class BaseEventImpl;
class BaseEvent {
  friend BaseEventImpl;
  friend void pushEvent(const BaseEvent& event);
  public:
    explicit BaseEvent(BaseEventImpl* impl);
    virtual ~BaseEvent() noexcept {};
    virtual void operator()(const BaseEventHandler& abstractHandler) const = 0;

  protected:
    std::unique_ptr<BaseEventImpl, void(*) (BaseEventImpl *)> _impl;
};

void pushEvent(const BaseEvent& event);

class UserEvent : public BaseEvent {
  public:
    UserEvent();
    uint16_t userNumber;
    virtual void operator()(const BaseEventHandler& abstractHandler) const {
      castHandler(*this, abstractHandler);
    }
};

class SystemEvent : public BaseEvent {
  public:
    SystemEvent();
    uint16_t systemNumber;
    virtual void operator()(const BaseEventHandler& abstractHandler) const {
      castHandler(*this, abstractHandler);
    };
};

class CustomEventImpl;
class CustomEvent : public BaseEvent {
  friend CustomEventImpl;
  public:
    CustomEvent();
    CustomEvent(CustomEventImpl* impl);
    virtual ~CustomEvent() {};
    
    uint16_t customEventNumber;
    void* payload;
    virtual void operator()(const BaseEventHandler& abstractHandler) const {
      castHandler(*this, abstractHandler);
    };

    virtual CustomEvent& clone() const;
    virtual CustomEventImpl* cloneImpl() const;

};

template<class EventClass>
class EventHandler: public BaseEventHandler, public InternalEventHandler<EventClass> {};

BaseEvent& getEvent();

#endif
