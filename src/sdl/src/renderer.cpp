#include "renderer.h"

#include <SDL3/SDL.h>

#include <cinttypes>
#include <memory>

#include "exception.h"
#include "float_rectangle_impl.h"
#include "renderer_impl.h"
#include "texture_impl.h"
#include "window.h"
#include "window_impl.h"

namespace sdlpp {

Renderer::Renderer(Window& window, const char* name)
    : _rendererImpl{std::make_unique<RendererImpl>()} {
  _rendererImpl->_sdlWindow = window._windowImpl->_sdlWindow;

  _rendererImpl->_sdlRenderer =
      SDL_CreateRenderer(_rendererImpl->_sdlWindow, name);
  if (_rendererImpl->_sdlRenderer == nullptr) {
    throw Exception("SDL_CreateRenderer");
  }
}

Renderer::Renderer(Renderer&& other) noexcept
    : _rendererImpl{std::move(other._rendererImpl)} {};

Renderer::~Renderer() noexcept {
  if (_rendererImpl && _rendererImpl->_sdlRenderer != nullptr) {
    SDL_DestroyRenderer(_rendererImpl->_sdlRenderer);
  }
}

auto Renderer::operator=(Renderer&& other) noexcept -> Renderer& {
  if (this == &other) {
    return *this;
  }
  if (_rendererImpl && _rendererImpl->_sdlRenderer != nullptr) {
    SDL_DestroyRenderer(_rendererImpl->_sdlRenderer);
  }
  _rendererImpl = std::move(other._rendererImpl);
  return *this;
}

void Renderer::setRenderDrawColour(const Color& color) {
  auto retVal = SDL_SetRenderDrawColor(_rendererImpl->_sdlRenderer,
                                       color.getRed(), color.getGreen(),
                                       color.getBlue(), color.getAlpha());
  if (!retVal) {
    throw Exception("SDL_SetRenderDrawColor");
  }
}

void Renderer::copy(const Texture& texture) {
  auto returnValue =
      SDL_RenderTexture(_rendererImpl->_sdlRenderer,
                        texture._textureImpl->_sdlTexture, nullptr, nullptr);
  if (!returnValue) {
    throw Exception("SDL_RenderCopy");
  }
}

void Renderer::copy(const Texture& texture, const FloatRectangle& source,
                    const FloatRectangle& destination) {
  SDL_FRect* sourceRect = source._rectangleImpl->getSDLRect();
  SDL_FRect* destRect = destination._rectangleImpl->getSDLRect();

  auto returnValue = SDL_RenderTexture(_rendererImpl->_sdlRenderer,
                                       texture._textureImpl->_sdlTexture,
                                       sourceRect, destRect);
  if (!returnValue) {
    throw Exception("SDL_RenderCopy");
  }
}

void Renderer::clear() const {
  auto retVal = SDL_RenderClear(_rendererImpl->_sdlRenderer);
  if (!retVal) {
    throw Exception("SDL_SetRenderClear");
  }
}

void Renderer::present() const {
  SDL_RenderPresent(_rendererImpl->_sdlRenderer);
}


}  // namespace sdlpp
