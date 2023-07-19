#include <cinttypes>
#include <memory>

#include <SDL2/SDL.h>

#include "exception.h"
#include "window.h"

#include "renderer.h"
#include "window_impl.h"

namespace sdl {

class RendererImpl {
  friend Renderer;
  private:
    // This is ownned by the Window which was passed in - i.e. not us or the Renderer
    SDL_Window* _sdl_window;
    // Owning Renderer class owns the SDL_Renderer
    SDL_Renderer* _sdl_renderer;

};

Renderer::Renderer(Window& window, int16_t index, uint32_t flags) 
  : _rendererImpl { std::make_unique<RendererImpl>() } {
  
  _rendererImpl->_sdl_window = window._windowImpl->_sdl_window;
  _rendererImpl->_sdl_renderer = SDL_CreateRenderer(_rendererImpl->_sdl_window, index, flags);
  if(_rendererImpl->_sdl_renderer == nullptr) throw Exception("SDL_CreateRendere");
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
  if(this == &other) return *this;
  if(_rendererImpl->_sdl_renderer != nullptr) SDL_DestroyRenderer(_rendererImpl->_sdl_renderer);
  _rendererImpl->_sdl_renderer = other._rendererImpl->_sdl_renderer;
  other._rendererImpl->_sdl_renderer = nullptr;
  return *this;
}

Renderer::~Renderer() noexcept {
  SDL_DestroyRenderer(_rendererImpl->_sdl_renderer);
}

void Renderer::setRenderDrawColour(const Color& color) {
  auto retVal = SDL_SetRenderDrawColor(
    _rendererImpl->_sdl_renderer, 
    color.getRed(), 
    color.getGreen(), 
    color.getBlue(), 
    color.getAlpha() );
  if (retVal < 0) throw Exception("SDL_SetRenderDrawColor");
}

void Renderer::clear() {
  auto retVal = SDL_RenderClear(_rendererImpl->_sdl_renderer);
  if (retVal < 0) throw Exception("SDL_SetRenderClear");
}

void Renderer::present() {
  SDL_RenderPresent(_rendererImpl->_sdl_renderer);
}

}
