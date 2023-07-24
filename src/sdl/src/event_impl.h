#include <atomic>
#include <functional>
#include <forward_list>
#include <vector>

#include "constexpr_map.h"

#include "event.h"

namespace sdl {

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

std::unique_ptr<QuitEvent> createQuitEvent(const SDL_QuitEvent* sdlQuitEvent);
std::unique_ptr<MouseButtonEvent> createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent);

}
