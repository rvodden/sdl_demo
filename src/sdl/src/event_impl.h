#include <atomic>
#include <functional>
#include <forward_list>
#include <vector>

#include "constexpr_map.h"

#include "event.h"

namespace sdl {

static constexpr vodden::Map<EventType, uint32_t, 3> sdlEventTypeMap {{
  { SDL_EventType::SDL_QUIT, kQuit },
  { SDL_EventType::SDL_MOUSEBUTTONDOWN, kMouseButtonEvent },
  { SDL_EventType::SDL_MOUSEBUTTONUP, kMouseButtonEvent }
}};

static constexpr vodden::Map<uint32_t, MouseButtonEvent::Button, 5> sdlMouseButtonEventButtonMap {{
  { SDL_BUTTON_LEFT, MouseButtonEvent::Button::kLeft },
  { SDL_BUTTON_MIDDLE, MouseButtonEvent::Button::kMiddle },
  { SDL_BUTTON_RIGHT, MouseButtonEvent::Button::kRight },
  { SDL_BUTTON_X1, MouseButtonEvent::Button::kX1 },
  { SDL_BUTTON_X2, MouseButtonEvent::Button::kX2 },
}};

static constexpr vodden::Map<uint32_t, MouseButtonEvent::State, 2> sdlMouseButtonEventStateMap {{
  { SDL_PRESSED, MouseButtonEvent::State::kPressed },
  { SDL_RELEASED, MouseButtonEvent::State::kReleased },
}};

class EventProcessorImpl;

class DefaultQuitEventHandler : public EventHandler<QuitEvent>, public BaseEventHandler {
  public:
    DefaultQuitEventHandler(EventProcessorImpl& eventProcessorImpl ) : _eventProcessorImpl { eventProcessorImpl } {}
    virtual void handle( [[maybe_unused]] const QuitEvent& quitEvent );
  private:
    EventProcessorImpl& _eventProcessorImpl;
};

class EventProcessorImpl {
  friend EventProcessor;
  public:
    void quit() { quitFlag = true; };
  private:
    std::forward_list<std::reference_wrapper<BaseEventHandler>> _eventHandlers {};
    std::atomic_bool quitFlag { false };
    DefaultQuitEventHandler defaultQuitEventHandler { *this };
};

QuitEvent createQuitEvent(const SDL_QuitEvent* sdlQuitEvent);
MouseButtonEvent createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent);

}
