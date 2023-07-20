#include <cinttypes>
#include <memory>

#include <SDL2/SDL.h>

#include "exception.h"
#include "window.h"

#include "renderer.h"

#include "window_impl.h"
#include "renderer_impl.h"

namespace sdl {

Renderer::Renderer(Window& window, int16_t index, std::unordered_set<RendererFlag> flags) 
  : _rendererImpl { std::make_unique<RendererImpl>() } {
  
  _rendererImpl->_sdl_window = window._windowImpl->_sdl_window;

  uint32_t flagValue = 0;
  for(const RendererFlag& flag : flags) flagValue |= sdlRendererFlagMap[flag];

  _rendererImpl->_sdl_renderer = SDL_CreateRenderer(_rendererImpl->_sdl_window, index, flagValue);
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
