#ifndef SDL_TOOLS_BUTTON_IMPL_H
#define SDL_TOOLS_BUTTON_IMPL_H

#include <forward_list>

#include <event.h>
#include <float_rectangle.h>

#include "button.h"

namespace sdl::tools {

class ButtonImpl {
  friend Button;
  public:
    ButtonImpl(std::shared_ptr<EventDispatcher> eventProcessor, sdl::FloatRectangle rectangle) : 
      _rectangle { std::move(rectangle) },
      _eventProcessor { std::move(eventProcessor) } { };

    class MouseEventHandler : public sdl::EventHandler<sdl::MouseButtonEvent>, public sdl::BaseEventHandler {
      public:
        MouseEventHandler(
          FloatRectangle& rectangle,
          std::forward_list<Button::Handler>& eventHandlers
        ) : _rectangle{ rectangle }, _eventHandlers { eventHandlers } { };
        void handle(const sdl::MouseButtonEvent& mouseButtonEvent) override;
      private:
        FloatRectangle _rectangle;
        std::forward_list<Button::Handler> _eventHandlers;
    };

  private:
    void eventHandler(const sdl::MouseButtonEvent &mouseButtonEvent);

    sdl::FloatRectangle _rectangle;
    std::shared_ptr<EventDispatcher> _eventProcessor;
    std::forward_list<Button::Handler> _eventHandlers;
    MouseEventHandler _mouseEventHandler { _rectangle , _eventHandlers };
};

}



#endif
