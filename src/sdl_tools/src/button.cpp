#include <iostream>
#include <memory>

#include "button_impl.h"
#include "button.h"

namespace sdl::tools {

void ButtonImpl::MouseEventHandler::handle([[maybe_unused]] const sdl::MouseButtonEvent &mouseButtonEvent) {
  if(_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y) ) {
    for(auto& handler : _eventHandlers) handler(mouseButtonEvent);
  }
}

Button::Button(EventDispatcher &eventProcessor, sdl::Rectangle rectangle) : _buttonImpl { std::make_unique<ButtonImpl>(eventProcessor, rectangle) } {
  _buttonImpl->_eventProcessor.registerEventHandler(_buttonImpl->_mouseEventHandler);
}

Button::Button(Button &&other) : _buttonImpl { std::move(other._buttonImpl) } {};

Button::~Button() {}

void Button::registerEventHandler(Handler handler) {
  _buttonImpl->_eventHandlers.push_front(handler);
};

void ButtonImpl::eventHandler([[maybe_unused]] const sdl::MousePositionEvent &mousePositionEvent)
{
  if(_rectangle.contains(mousePositionEvent.x, mousePositionEvent.y)) {
    for(const auto& handler : _eventHandlers) {
      handler(mousePositionEvent);
    }
  };
}

}
