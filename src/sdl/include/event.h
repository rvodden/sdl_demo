#ifndef __SDL_EVENT_H__
#define __SDL_EVENT_H__

#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>

namespace sdl {

class UnknownEventException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

//! @brief a superclass for all event handler
class BaseEventHandler {
  public:
    virtual ~BaseEventHandler() {};
};

template <class EventClass>
class EventHandler {
  public:
    virtual ~EventHandler() {};
    virtual void handle(const EventClass& event) = 0;
};

//! @brief a superclass for all events
class BaseEvent {
  public:
    virtual ~BaseEvent() {};
    virtual void handle(BaseEventHandler &baseEventHandler) = 0;
};

template <class EventClass>
void castHandler(const EventClass& eventClass, BaseEventHandler& baseEventHandler) {
  try {
    EventHandler<EventClass> &eventHandler = dynamic_cast<EventHandler<EventClass>&>(baseEventHandler);
    eventHandler.handle(eventClass);
  } catch (std::bad_cast &e) { } // bad cast just means this handler can't handle this event
}

class Event : public BaseEvent {
  public:
    //! @brief constructor
    Event(std::chrono::duration<uint64_t, std::milli> ts) : timestamp{ts} {};
    //! @brief timestamp of the event
    std::chrono::duration<uint64_t, std::milli> timestamp;

    virtual void handle(BaseEventHandler &baseEventHandler) override {
      castHandler(*this, baseEventHandler);
    };
};

class QuitEvent : public Event {
  public:
    using Event::Event;
    virtual void handle(BaseEventHandler &baseEventHandler ) override {
      castHandler(*this, baseEventHandler);
    };
};

class MouseEvent : public Event {
  public:
    MouseEvent(
      std::chrono::duration<int64_t, 
      std::milli> ts,
      uint32_t windowId,
      u_int32_t which
    ) : Event(ts), windowId { windowId }, which { which } {};
    
    virtual void handle(BaseEventHandler &baseEventHandler) override {
      castHandler(*this, baseEventHandler);
    };
    
    //! @brief the window with mouse focus, if any
    uint32_t windowId;
    
    //! @brief the mouse instance id
    uint32_t which;
};

class MousePositionEvent : public MouseEvent {
  public:
    MousePositionEvent(
      std::chrono::duration<int64_t, std::milli> ts, 
      uint32_t windowId, 
      uint32_t which,
      int32_t x,
      int32_t y
    ) : MouseEvent(ts, windowId, which), x { x }, y { y } {};
    
    virtual void handle(BaseEventHandler &baseEventHandler) override {
      castHandler(*this, baseEventHandler);
    };
    
    //! @brief x co-ordinate of event relative to window.
    int32_t x;
    //! @brief y co-ordinate of event relative to window.
    int32_t y;
};

class MouseButtonEvent : public MousePositionEvent {
  public:
    enum class State {
      kPressed,
      kReleased
    };

    enum class Button {
      kLeft,
      kMiddle,
      kRight,
      kX1,
      kX2
    };

    MouseButtonEvent(
      std::chrono::duration<int64_t, std::milli> ts,
      uint32_t windowId, 
      uint32_t which,
      int32_t x,
      int32_t y,
      Button button,
      State state,
      uint8_t clicks
    ) : MousePositionEvent(ts, windowId, which, x, y) , button { button }, state{ state } , clicks { clicks } { };

    virtual void handle(BaseEventHandler &baseEventHandler) override {
      castHandler(*this, baseEventHandler);
    };
    
    //! @brief the mouse button which has changed state
    Button button;

    //! @brief the state to which the mouse button has changed
    State state;

    //! @brief 1 for single click, 2 for double click etc..
    uint8_t clicks;
};

class BaseEventProducer {
  public:
    virtual std::unique_ptr<BaseEvent> wait() = 0;
    virtual void produce(std::unique_ptr<Event>) {};
};

class EventProducer : public BaseEventProducer {
  public:
    virtual std::unique_ptr<BaseEvent> wait();
    virtual void produce(std::unique_ptr<Event>) {};
};


}

#endif
