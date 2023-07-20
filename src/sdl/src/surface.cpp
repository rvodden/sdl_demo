#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "exception.h"

#include "surface_impl.h"
#include "surface.h"

namespace sdl {


Surface::Surface(uint32_t width, uint32_t height, uint8_t depth, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, uint32_t alphaMask) {
  _surfaceImpl->_sdlSurface = SDL_CreateRGBSurface(0, width, height, depth, redMask, greenMask, blueMask, alphaMask);
  if (_surfaceImpl->_sdlSurface == nullptr) throw Exception("SDL_CreateRGBSurface");
}

Surface::Surface(std::filesystem::path filePath) {
  _surfaceImpl->_sdlSurface = IMG_Load(filePath.c_str());
}

Surface::~Surface() {
  if(_surfaceImpl->_sdlSurface != nullptr)
    SDL_FreeSurface(_surfaceImpl->_sdlSurface);
}

}
