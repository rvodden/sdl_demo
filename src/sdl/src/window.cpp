#include "window.h"

#include <SDL3/SDL.h>

#include <memory>

#include "exception.h"
#include "renderer.h"
#include "window_impl.h"

namespace sdl {

Window::Window(std::string title, uint16_t width, uint16_t height,
               uint32_t flags)
    : _title{std::move(title)}, _windowImpl{std::make_unique<WindowImpl>()} {
  _windowImpl->_sdlWindow =
      SDL_CreateWindow(_title.c_str(), width, height, flags);
  if (_windowImpl->_sdlWindow == nullptr) {
    throw Exception("SDL_CreateWindow");
  }
}

Window::Window(Window&& other) noexcept
    : _title{std::move(other._title)}, _windowImpl{std::move(other._windowImpl)} {}

Window::~Window() {
  if (_windowImpl && _windowImpl->_sdlWindow != nullptr) {
    SDL_DestroyWindow(_windowImpl->_sdlWindow);
  }
}

auto Window::operator=(Window&& other) noexcept -> Window& {
  if (&other == this) {
    return *this;
  }
  if (_windowImpl && _windowImpl->_sdlWindow != nullptr) {
    SDL_DestroyWindow(_windowImpl->_sdlWindow);
  }
  _title = std::move(other._title);
  _windowImpl = std::move(other._windowImpl);
  return *this;
}

auto Window::getTitle() -> std::string_view {
  return {SDL_GetWindowTitle(_windowImpl->_sdlWindow)};
}

void Window::setTitle(const std::string& newTitle) {
  SDL_SetWindowTitle(_windowImpl->_sdlWindow, newTitle.c_str());
}

}  // namespace sdl
