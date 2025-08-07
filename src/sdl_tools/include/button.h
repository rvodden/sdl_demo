#ifndef __SDL_TOOLS_BUTTON_H__
#define __SDL_TOOLS_BUTTON_H__

#include <memory>

#include <event.h>
#include <float_rectangle.h>

#include <event_dispatcher.h>

namespace sdl::tools {

class ButtonImpl;

class Button {
  public:
    typedef std::function<void(const MouseButtonEvent&)> Handler;

    Button(EventDispatcher& eventProcessor, sdl::FloatRectangle rectangle);
    Button(Button&& other);
    ~Button();

    void registerEventHandler(Handler handler);

  private:
    std::unique_ptr<ButtonImpl> _buttonImpl;
};

}

#endif
