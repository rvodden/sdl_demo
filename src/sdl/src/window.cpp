#include <memory>

#include <SDL3/SDL.h>

#include "sdl/exception.h"
#include "sdl/renderer.h"
#include "sdl/window.h"

#include "window_impl.h"

namespace sdl {

Window::Window(std::string title, uint16_t width, uint16_t height,
               uint32_t flags)
    : _title{std::move(title)}, _impl{std::make_unique<WindowImpl>()} {
  _impl->_sdlWindow =
      SDL_CreateWindow(_title.c_str(), width, height, flags);
  if (_impl->_sdlWindow == nullptr) {
    throw Exception("SDL_CreateWindow");
  }
}

Window::Window(Window&& other) noexcept
    : _title{std::move(other._title)}, _impl{std::move(other._impl)} {}

Window::~Window() {
  if (_impl && _impl->_sdlWindow != nullptr) {
    SDL_DestroyWindow(_impl->_sdlWindow);
  }
}

auto Window::operator=(Window&& other) noexcept -> Window& {
  if (&other == this) {
    return *this;
  }
  if (_impl && _impl->_sdlWindow != nullptr) {
    SDL_DestroyWindow(_impl->_sdlWindow);
  }
  _title = std::move(other._title);
  _impl = std::move(other._impl);
  return *this;
}

auto Window::getTitle() -> std::string_view {
  return {SDL_GetWindowTitle(_impl->_sdlWindow)};
}

void Window::setTitle(const std::string& newTitle) {
  SDL_SetWindowTitle(_impl->_sdlWindow, newTitle.c_str());
}

auto Window::getImpl() const -> const WindowImpl& {
  return *_impl;
}

}  // namespace sdl
