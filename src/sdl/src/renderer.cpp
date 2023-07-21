#include <cinttypes>
#include <memory>

#include <SDL2/SDL.h>

#include "exception.h"
#include "window.h"

#include "renderer.h"

#include "window_impl.h"
#include "rectangle_impl.h"
#include "renderer_impl.h"
#include "texture_impl.h"

namespace sdl {

Renderer::Renderer(Window& window, int16_t index, std::unordered_set<RendererFlag> flags) 
  : _rendererImpl { std::make_unique<RendererImpl>() } {
  
  _rendererImpl->_sdlWindow = window._windowImpl->_sdlWindow;

  uint32_t flagValue = 0;
  for(const RendererFlag& flag : flags) flagValue |= sdlRendererFlagMap[flag];

  _rendererImpl->_sdlRenderer = SDL_CreateRenderer(_rendererImpl->_sdlWindow, index, flagValue);
  if(_rendererImpl->_sdlRenderer == nullptr) throw Exception("SDL_CreateRendere");
}

Renderer::Renderer(Renderer&& other) noexcept : _rendererImpl { std::move(other._rendererImpl) } { };

Renderer::~Renderer() noexcept {
  SDL_DestroyRenderer(_rendererImpl->_sdlRenderer);
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
  if(this == &other) return *this;
  if(_rendererImpl->_sdlRenderer != nullptr) SDL_DestroyRenderer(_rendererImpl->_sdlRenderer);
  _rendererImpl = std::move(other._rendererImpl);
  return *this;
}

void Renderer::setRenderDrawColour(const Color& color) {
  auto retVal = SDL_SetRenderDrawColor(
    _rendererImpl->_sdlRenderer, 
    color.getRed(), 
    color.getGreen(), 
    color.getBlue(), 
    color.getAlpha() );
  if (retVal < 0) throw Exception("SDL_SetRenderDrawColor");
}

Renderer &Renderer::copy(Texture &texture)
{
  auto returnValue = SDL_RenderCopy(_rendererImpl->_sdlRenderer, texture._textureImpl->_sdlTexture, nullptr, nullptr);
  if(returnValue < 0) throw Exception("SDL_RenderCopy");

  return *this;
}

Renderer &Renderer::copy(Texture &texture, const Rectangle &source, const Rectangle &destination) {
  SDL_Rect* sourceRect = source._rectangleImpl->getSDLRect();
  SDL_Rect* destRect = destination._rectangleImpl->getSDLRect();

  auto returnValue = SDL_RenderCopy(_rendererImpl->_sdlRenderer, texture._textureImpl->_sdlTexture, sourceRect, destRect);
  if(returnValue < 0) throw Exception("SDL_RenderCopy");

  return *this;
}

void Renderer::clear() {
  auto retVal = SDL_RenderClear(_rendererImpl->_sdlRenderer);
  if (retVal < 0) throw Exception("SDL_SetRenderClear");
}

void Renderer::present() {
  SDL_RenderPresent(_rendererImpl->_sdlRenderer);
}

}
