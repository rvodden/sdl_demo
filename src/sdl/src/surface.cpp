#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "exception.h"

#include "surface_impl.h"
#include "surface.h"

namespace sdl {


Surface::Surface(uint32_t width, uint32_t height, uint8_t depth, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, uint32_t alphaMask) {
  // TODO: add overloaded constructor which takes format
  _surfaceImpl->_sdlSurface = SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_GetPixelFormatForMasks(depth, redMask, greenMask, blueMask, alphaMask));
  if (_surfaceImpl->_sdlSurface == nullptr) { throw Exception("SDL_CreateRGBSurface"); }
}

Surface::Surface(const std::filesystem::path& filePath) {
  _surfaceImpl->_sdlSurface = IMG_Load(filePath.c_str());
}

Surface::Surface(Surface&& other) noexcept : _surfaceImpl( std::move(other._surfaceImpl) ) {};

Surface::~Surface() {
  if(_surfaceImpl->_sdlSurface != nullptr) {
    SDL_DestroySurface(_surfaceImpl->_sdlSurface);
  }
}

auto Surface::operator=(Surface &&other) noexcept -> Surface& {
  if(this == &other) { return *this; }
  if(_surfaceImpl->_sdlSurface != nullptr) { SDL_DestroySurface(_surfaceImpl->_sdlSurface); }
  _surfaceImpl = std::move(other._surfaceImpl);
  return *this;
}

}
