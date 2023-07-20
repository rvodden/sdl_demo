#include <atomic>
#include <functional>
#include <unordered_map>
#include <vector>

#include "event_processor.h"

namespace sdl {

static constexpr vodden::Map<EventType, uint32_t, 1> sdlEventTypeMap {{
  { SDL_EventType::SDL_QUIT, kQuit }
}};


class EventProcessorImpl {
  friend EventProcessor;
  private:
    std::unordered_map<EventType,std::vector<EventProcessor::Handler>> _eventHandlers {};
    std::atomic_bool quitFlag { false };
};

QuitEvent createQuitEvent(const SDL_QuitEvent* quitEvent);

}
