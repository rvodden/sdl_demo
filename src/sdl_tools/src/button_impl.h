#ifndef __SDL_TOOLS_BUTTON_IMPL_H__
#define __SDL_TOOLS_BUTTON_IMPL_H__

#include <forward_list>

#include <event.h>
#include <rectangle.h>

#include "button.h"

namespace sdl::tools {

class ButtonImpl {
  friend Button;
  public:
    ButtonImpl(EventDispatcher &eventProcessor, sdl::Rectangle& rectangle) : 
      _rectangle { rectangle },
      _eventProcessor { eventProcessor } { };

    class MouseEventHandler : public sdl::EventHandler<sdl::MouseButtonEvent>, public sdl::BaseEventHandler {
      public:
        MouseEventHandler(
          Rectangle& rectangle,
          std::forward_list<Button::Handler>& eventHandlers
        ) : _rectangle{ rectangle }, _eventHandlers { eventHandlers } { };
        virtual void handle(const sdl::MouseButtonEvent& mouseEvent);
      private:
        Rectangle& _rectangle;
        std::forward_list<Button::Handler>& _eventHandlers;
    };

  private:
    void eventHandler(const sdl::MousePositionEvent &mousePositionEvent);

    sdl::Rectangle _rectangle;
    EventDispatcher& _eventProcessor;
    std::forward_list<Button::Handler> _eventHandlers { };
    MouseEventHandler _mouseEventHandler { _rectangle , _eventHandlers };
};

}



#endif
