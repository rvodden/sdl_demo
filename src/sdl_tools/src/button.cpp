#include "button.h"

#include <iostream>
#include <memory>

#include "button_impl.h"

namespace sdl::tools {

void ButtonImpl::MouseEventHandler::handle(
    [[maybe_unused]] const sdl::MouseButtonEvent& mouseButtonEvent) {
  if (_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y)) {
    for (auto& handler : *_eventHandlers) {
      handler(mouseButtonEvent);
    }
  }
}

Button::Button(std::shared_ptr<EventRouter> eventProcessor,
               sdl::Rectangle<float> rectangle)
    : _impl{std::make_unique<ButtonImpl>(std::move(eventProcessor),
                                               std::move(rectangle))} {
  _impl->_eventRouter->registerEventHandler(
      _impl->_mouseEventHandler);
}

Button::Button(Button&& other) noexcept
    : _impl{std::move(other._impl)} {};

Button::~Button() = default;

auto Button::operator=(Button&& other) noexcept -> Button& {
  std::swap(_impl, other._impl);
  return *this;
}

void Button::registerEventHandler(Handler&& handler) {
  _impl->_eventHandlers->emplace_back(std::forward<Handler>(handler));
};

void ButtonImpl::eventHandler(
    [[maybe_unused]] const sdl::MouseButtonEvent& mouseButtonEvent) {
  if (_rectangle.contains(mouseButtonEvent.x, mouseButtonEvent.y)) {
    for (const auto& handler : *_eventHandlers) {
      handler(mouseButtonEvent);
    }
  };
}

}  // namespace sdl::tools
