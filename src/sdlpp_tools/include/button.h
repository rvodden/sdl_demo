#ifndef SDL_TOOLS_BUTTON_H
#define SDL_TOOLS_BUTTON_H

#include <event.h>
#include <event_router.h>
#include <float_rectangle.h>

#include <memory>

#include "sdlpp_tools_export.h"

namespace sdlpp::tools {

class ButtonImpl;

class SDLPP_TOOLS_EXPORT Button {
 public:
  using Handler = std::function<void(const MouseButtonEvent&)>;

  Button(std::shared_ptr<EventRouter> eventRouter,
         sdlpp::FloatRectangle rectangle);
  Button(const Button& other) = delete;
  Button(Button&& other) noexcept;

  auto operator=(const Button& other) -> Button& = delete;
  auto operator=(Button&& other) noexcept -> Button&;

  ~Button();

  void registerEventHandler(const Handler& handler);

 private:
  std::unique_ptr<ButtonImpl> _buttonImpl;
};

}  // namespace sdlpp::tools

#endif
