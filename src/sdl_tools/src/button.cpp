#include "button.h"

#include <iostream>
#include <memory>

#include "button_impl.h"

namespace sdlpp::tools {

void ButtonImpl::MouseEventHandler::handle(
    [[maybe_unused]] const sdlpp::MouseButtonEvent& mouseButtonEvent) {
  if (_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y)) {
    for (auto& handler : *_eventHandlers) {
      handler(mouseButtonEvent);
    }
  }
}

Button::Button(std::shared_ptr<EventDispatcher> eventProcessor,
               FloatRectangle rectangle)
    : _buttonImpl{std::make_unique<ButtonImpl>(std::move(eventProcessor),
                                               std::move(rectangle))} {
  _buttonImpl->_eventDispatcher->registerEventHandler(
      _buttonImpl->_mouseEventHandler);
}

Button::Button(Button&& other) noexcept
    : _buttonImpl{std::move(other._buttonImpl)} {};

Button::~Button() = default;

auto Button::operator=(Button&& other) noexcept -> Button& {
  std::swap(_buttonImpl, other._buttonImpl);
  return *this;
}

void Button::registerEventHandler(const Handler& handler) {
  _buttonImpl->_eventHandlers->push_front(handler);
};

void ButtonImpl::eventHandler(
    [[maybe_unused]] const sdlpp::MouseButtonEvent& mouseButtonEvent) {
  if (_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y)) {
    for (const auto& handler : *_eventHandlers) {
      handler(mouseButtonEvent);
    }
  };
}

}  // namespace sdlpp::tools
