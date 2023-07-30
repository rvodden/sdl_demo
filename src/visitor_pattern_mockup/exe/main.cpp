#include <functional>
#include <vector>

#include "visitor_pattern_legacy_sdk.h"

class ConcreteCustomEvent: public CustomEventTemplate<ConcreteCustomEvent> {
  public:
    ConcreteCustomEvent(uint16_t customEventNumber, std::string message):
      customEventNumber { customEventNumber },
      message { message }
    { };

    uint16_t customEventNumber;
    std::string message;
};

class UserEventHandler: public EventHandler<UserEvent> {
  public:
    virtual void handle(const UserEvent& userEvent) const {
      std::cout << "I am a user event with event number: " << userEvent.userNumber << std::endl;
    };
};

class SystemEventHandler: public EventHandler<SystemEvent> {
  public:
    virtual void handle(const SystemEvent& systemEvent) const {
      std::cout << "I am handling a system event: " << systemEvent.systemNumber << std::endl;
    };
};

class ConcreteCustomEventHandler: public EventHandler<ConcreteCustomEvent> {
  public:
    virtual void handle(const ConcreteCustomEvent& customEvent) const {
      std::cout << customEvent.message << ": " << customEvent.customEventNumber << std::endl;
    };
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {

  UserEventHandler userEventHandler;
  SystemEventHandler systemEventHandler;
  ConcreteCustomEventHandler customEventHandler;

  std::vector<std::reference_wrapper<BaseEventHandler>> eventHandlers {
    std::ref<UserEventHandler>(userEventHandler),
    std::ref<SystemEventHandler>(systemEventHandler),
    std::ref<ConcreteCustomEventHandler>(customEventHandler)
  };

  std::vector<std::reference_wrapper<BaseEvent>> newEvents;
  try {
    while(true) {
      newEvents.emplace_back(getEvent());
    }
  } catch (NoEventsException& _) {};

  while(std::size(newEvents) > 0) {
    BaseEvent& newEvent = std::move(newEvents.back());
    newEvents.pop_back();
    for(const BaseEventHandler& handler : eventHandlers) {
      newEvent.acceptHandler(handler);
    }
    delete &newEvent;
  }

  ConcreteCustomEvent customEvent { 17, "your mum!" };

  pushEvent(customEvent);

  try {
    while(true) {
      newEvents.push_back(getEvent());
    }
  } catch (NoEventsException& _) {};

  while(std::size(newEvents) > 0) {
    const BaseEvent& newEvent = newEvents.back();
    for(const BaseEventHandler& handler : eventHandlers) {
      newEvent.acceptHandler(handler);
    }
    newEvents.pop_back();
    delete &newEvent;
  }

}
