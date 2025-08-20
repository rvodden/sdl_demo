#ifndef SDL_TOOLS_BUTTON_H
#define SDL_TOOLS_BUTTON_H

#include <event.h>
#include <event_dispatcher.h>
#include <float_rectangle.h>

#include <memory>

#include "sdl_tools_export.h"

namespace sdl::tools {

class ButtonImpl;

class SDL_TOOLS_EXPORT Button {
 public:
  using Handler = std::function<void(const MouseButtonEvent&)>;

  Button(std::shared_ptr<EventDispatcher> eventProcessor,
         sdl::FloatRectangle rectangle);
  Button(const Button& other) = delete;
  Button(Button&& other) noexcept;

  auto operator=(const Button& other) -> Button& = delete;
  auto operator=(Button&& other) noexcept -> Button&;

  ~Button();

  void registerEventHandler(const Handler& handler);

 private:
  std::unique_ptr<ButtonImpl> _buttonImpl;
};

}  // namespace sdl::tools

#endif
