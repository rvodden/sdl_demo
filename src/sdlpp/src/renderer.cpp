#include "renderer.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <memory>

#include "exception.h"
#include "rectangle_impl.h"
#include "renderer_impl.h"
#include "texture_impl.h"
#include "window.h"
#include "window_impl.h"

namespace sdlpp {

Renderer::Renderer(Window& window)
    : _rendererImpl{std::make_unique<RendererImpl>()} {
  _rendererImpl->_sdlWindow = window._windowImpl->_sdlWindow;
  
  if (_rendererImpl->_sdlWindow == nullptr) {
    throw Exception("Attempting to create Renderer from null window.");
  }
  _rendererImpl->_sdlRenderer =
      SDL_CreateRenderer(_rendererImpl->_sdlWindow, nullptr);
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

void Renderer::setDrawColour(const Color& color) {
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

void Renderer::copy(const Texture& texture, const Rectangle<float>& source,
                    const Rectangle<float>& destination) {
  SDL_FRect* sourceRect = source.impl_->getSDLRect();
  SDL_FRect* destRect = destination.impl_->getSDLRect();

  auto returnValue = SDL_RenderTexture(_rendererImpl->_sdlRenderer,
                                       texture._textureImpl->_sdlTexture,
                                       sourceRect, destRect);
  if (!returnValue) {
    throw Exception("SDL_RenderCopy");
  }
}

void Renderer::copy(const Texture& texture, 
                    const Rectangle<float>& destination) {
  SDL_FRect* destRect = destination.impl_->getSDLRect();

  auto returnValue = SDL_RenderTexture(_rendererImpl->_sdlRenderer,
                                       texture._textureImpl->_sdlTexture,
                                       nullptr, destRect);
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

auto Renderer::readPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const -> std::vector<uint8_t> {
  SDL_Rect rect{static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height)};
  
  SDL_Surface* surface = SDL_RenderReadPixels(_rendererImpl->_sdlRenderer, &rect);
  if (surface == nullptr) {
    throw Exception("SDL_RenderReadPixels");
  }
  
  // Assume 4 bytes per pixel (RGBA) for simplicity
  std::vector<uint8_t> pixels;
  size_t pixelCount = static_cast<size_t>(surface->w * surface->h * 4);
  pixels.resize(pixelCount);
  
  if (SDL_LockSurface(surface)) {
    std::memcpy(pixels.data(), surface->pixels, std::min(pixelCount, static_cast<size_t>(surface->pitch * surface->h)));
    SDL_UnlockSurface(surface);
  }
  
  SDL_DestroySurface(surface);
  return pixels;
}

void Renderer::setScale(float xScale, float yScale) {
  auto success = SDL_SetRenderScale(_rendererImpl->_sdlRenderer, xScale, yScale);
  if(!success) { throw Exception("SDL_SetRenderScale"); }
}

auto Renderer::getOutputSize() -> Rectangle<int> {
  int width = 0;
  int height = 0;

  auto retVal = SDL_GetRenderOutputSize(_rendererImpl->_sdlRenderer, &width, &height);
  if (!retVal) {
    throw Exception("SDL_GetRenderOutputSize");
  }
  return { 0, 0, width, height };
}

}  // namespace sdlpp
