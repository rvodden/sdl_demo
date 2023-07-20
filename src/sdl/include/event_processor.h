#ifndef __SDL_EVENT_H__
#define __SDL_EVENT_H__

#include <chrono>
#include <functional>
#include <memory>

namespace sdl {

typedef uint32_t EventType;
static constexpr EventType kQuit = 0;

class EventProcessorImpl;

class Event {
  public:
    EventType type;
    std::chrono::duration<int64_t, std::milli> timestamp;
};

class QuitEvent : public Event {};

class EventProcessor {
  public:
    typedef std::function<void(const Event&)> Handler;

    EventProcessor();
    ~EventProcessor();
    void run();
    void registerEventHandler(const EventType& eventType, Handler handler);

  private:
    void defaultQuitEventHandler(const Event&);
    std::unique_ptr<EventProcessorImpl> _eventProcessorImpl;
};

}

#endif
