#ifndef SDL_TOOLS_BUTTON_IMPL_H
#define SDL_TOOLS_BUTTON_IMPL_H

#include <event.h>
#include <rectangle.h>

#include <vector>

#include "button.h"

namespace sdlpp::tools {

class ButtonImpl {
  friend Button;

 public:
  ButtonImpl(std::shared_ptr<EventRouter> eventRouter,
             const sdlpp::Rectangle<float>& rectangle)
      : _rectangle{rectangle},
        _eventRouter{std::move(eventRouter)},
        _eventHandlers{std::make_shared<std::vector<Button::Handler>>()},
        _mouseEventHandler{_rectangle, _eventHandlers} {};

  class MouseEventHandler : public sdlpp::EventHandler<sdlpp::MouseButtonEvent>,
                            public sdlpp::BaseEventHandler {
   public:
    MouseEventHandler(
        sdlpp::Rectangle<float>& rectangle,
        std::shared_ptr<std::vector<Button::Handler>> eventHandlers)
        : _rectangle{rectangle}, _eventHandlers{std::move(eventHandlers)} {};
    void handle(const sdlpp::MouseButtonEvent& mouseButtonEvent) override;

   private:
    sdlpp::Rectangle<float> _rectangle;
    std::shared_ptr<std::vector<Button::Handler>> _eventHandlers;
  };

 private:
  void eventHandler(const sdlpp::MouseButtonEvent& mouseButtonEvent);

  sdlpp::Rectangle<float> _rectangle;
  std::shared_ptr<EventRouter> _eventRouter;
  std::shared_ptr<std::vector<Button::Handler>> _eventHandlers;
  MouseEventHandler _mouseEventHandler;
};

}  // namespace sdlpp::tools

#endif
