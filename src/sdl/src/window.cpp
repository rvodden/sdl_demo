#include <memory>

#include <SDL2/SDL.h>

#include "exception.h"
#include "renderer.h"

#include "window.h"
#include "window_impl.h"

namespace sdl {

Window::Window(
  std::string title, 
  uint16_t x, 
  uint16_t y, 
  uint16_t width, 
  uint16_t height, 
  uint32_t flags
): _title { std::move(title) }, _windowImpl { std::make_unique<WindowImpl>() } {

    _windowImpl->_sdl_window = SDL_CreateWindow(_title.c_str(), x, y, width, height, flags);
    if (_windowImpl->_sdl_window == nullptr) throw Exception( "SDL_CreateWindow" );
}

Window::Window(Window&& other) noexcept {
  other._windowImpl->_sdl_window = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
  if(&other == this) return *this;
  if(_windowImpl->_sdl_window != nullptr) SDL_DestroyWindow( _windowImpl->_sdl_window );
  _windowImpl->_sdl_window = other._windowImpl->_sdl_window;
  other._windowImpl->_sdl_window = nullptr;
  return *this;
}

Window::~Window() {
  if(_windowImpl->_sdl_window != nullptr) SDL_DestroyWindow( _windowImpl->_sdl_window );
}

std::string_view Window::getTitle()
{
  return std::string_view(SDL_GetWindowTitle( _windowImpl->_sdl_window ));
}

void Window::setTitle(std::string newTitle)
{
  SDL_SetWindowTitle(_windowImpl->_sdl_window, newTitle.c_str());
}

}
