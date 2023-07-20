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

    _windowImpl->_sdlWindow = SDL_CreateWindow(_title.c_str(), x, y, width, height, flags);
    if (_windowImpl->_sdlWindow == nullptr) throw Exception( "SDL_CreateWindow" );
}

Window::Window(Window&& other) noexcept : _windowImpl { std::move(other._windowImpl ) } { }

Window::~Window() {
  if(_windowImpl->_sdlWindow != nullptr) SDL_DestroyWindow( _windowImpl->_sdlWindow );
}

Window& Window::operator=(Window &&other) noexcept {
  if(&other == this) return *this;
  if(_windowImpl->_sdlWindow != nullptr) SDL_DestroyWindow( _windowImpl->_sdlWindow );
  _windowImpl = std::move(other._windowImpl);
  return *this;
}

std::string_view Window::getTitle()
{
  return std::string_view(SDL_GetWindowTitle( _windowImpl->_sdlWindow ));
}

void Window::setTitle(std::string newTitle)
{
  SDL_SetWindowTitle(_windowImpl->_sdlWindow, newTitle.c_str());
}

}
