#include <iostream>
#include <memory>

#include "button_impl.h"
#include "button.h"

namespace sdl::tools {

void ButtonImpl::MouseEventHandler::handle([[maybe_unused]] const sdl::MouseButtonEvent &mouseButtonEvent) {
  std::cout << "MouseEventHandler::handle called with (" << mouseButtonEvent.x << "," << mouseButtonEvent.y << ") - checking rectangle (" << _rectangle.getX() << "," << _rectangle.getY() << "," << _rectangle.getWidth() << "," << _rectangle.getHeight() << ")\n";
  if(_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y) ) {
    std::cout << "Rectangle contains point - calling " << std::distance(_eventHandlers->begin(), _eventHandlers->end()) << " handlers\n";
    for(auto& handler : *_eventHandlers) { handler(mouseButtonEvent); }
  } else {
    std::cout << "Rectangle does not contain point\n";
  }
}

Button::Button(std::shared_ptr<EventDispatcher> eventProcessor, FloatRectangle rectangle) : _buttonImpl { std::make_unique<ButtonImpl>(std::move(eventProcessor), std::move(rectangle)) } {
  _buttonImpl->_eventProcessor->registerEventHandler(_buttonImpl->_mouseEventHandler);
}

Button::Button(Button &&other) noexcept : _buttonImpl { std::move(other._buttonImpl) } {};

Button::~Button() = default;

auto Button::operator=(Button&& other) noexcept -> Button&{
  std::swap(_buttonImpl, other._buttonImpl);
  return *this;
}

void Button::registerEventHandler(const Handler& handler) {
  _buttonImpl->_eventHandlers->push_front(handler);
};

void ButtonImpl::eventHandler([[maybe_unused]] const sdl::MouseButtonEvent &mouseButtonEvent)
{
  if(_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y)) {
    for(const auto& handler : *_eventHandlers) {
      handler(mouseButtonEvent);
    }
  };
}

}
