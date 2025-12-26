#ifndef SDL_TOOLS_BUTTON_IMPL_H
#define SDL_TOOLS_BUTTON_IMPL_H

#include <vector>

#include "sdl/button.h"
#include "sdl/event.h"
#include "sdl/event_registration.h"
#include "sdl/rectangle.h"

namespace sdl::tools {

class ButtonImpl {
  friend Button;

 public:
  ButtonImpl(std::shared_ptr<EventRouter> eventRouter,
             const sdl::Rectangle<float>& rectangle)
      : _rectangle{rectangle},
        _eventRouter{std::move(eventRouter)},
        _eventHandlers{std::make_shared<std::vector<Button::Handler>>()},
        _mouseEventHandler{_rectangle, _eventHandlers} {};

  class MouseEventHandler : public sdl::EventHandler<sdl::MouseButtonEvent>,
                            public sdl::BaseEventHandler {
   public:
    MouseEventHandler(
        sdl::Rectangle<float>& rectangle,
        std::shared_ptr<std::vector<Button::Handler>> eventHandlers)
        : _rectangle{rectangle}, _eventHandlers{std::move(eventHandlers)} {};
    void handle(const sdl::MouseButtonEvent& mouseButtonEvent) override;

   private:
    sdl::Rectangle<float> _rectangle;
    std::shared_ptr<std::vector<Button::Handler>> _eventHandlers;
  };

 private:
  void eventHandler(const sdl::MouseButtonEvent& mouseButtonEvent);

  sdl::Rectangle<float> _rectangle;
  std::shared_ptr<EventRouter> _eventRouter;
  std::shared_ptr<std::vector<Button::Handler>> _eventHandlers;
  MouseEventHandler _mouseEventHandler;
  EventRegistration _registration;
};

}  // namespace sdl::tools

#endif
