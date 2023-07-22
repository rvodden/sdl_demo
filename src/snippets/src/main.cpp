#include <vector>
#include <iostream>
#include <memory>
#include <string>

class Event;

class AbstractHandler { 
  public:
    virtual ~AbstractHandler() {};
};

template <class EventClass>
class EventHandler {
  public:
    virtual ~EventHandler() {};
    virtual void handle(const EventClass& event) = 0;
};

template <class EventClass>
void castHandler(const EventClass& eventClass, AbstractHandler& abstractHandler) {
  try {
    EventHandler<EventClass> &eventHandler = dynamic_cast<EventHandler<EventClass>&>(abstractHandler);
    eventHandler.handle(eventClass);
  } catch (std::bad_cast &e) { } // bad cast just means this handler can't handle this event
}

class Event {
  public:
    virtual void handle(AbstractHandler& abstractHandler) = 0;
    virtual std::string getName() const { return "Event"; }
    std::string name { "Event" };
};

class MouseEvent : public Event {
  public:
    virtual void handle(AbstractHandler& abstractHandler) {
      castHandler(*this, abstractHandler);
    }
    virtual std::string getName() const { return "MouseEvent"; }
};

class MouseEventHandler: public AbstractHandler, public EventHandler<MouseEvent> {
  public:
    void handle(const MouseEvent& mouseEvent) {
      std::cout << "I am handling a mouse event." << std::endl;
    }; //note no override
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  
  std::vector<std::unique_ptr<AbstractHandler>> eventHandlers;
  eventHandlers.push_back( std::make_unique<MouseEventHandler>() );

  for(const auto &eventHandler : eventHandlers) {
    MouseEvent().handle(*eventHandler);
  }

}
