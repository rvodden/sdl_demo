#include "texture.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "exception.h"
#include "renderer_impl.h"
#include "surface_impl.h"
#include "texture_impl.h"

namespace sdl {

Texture::Texture(const Renderer& renderer,
                 const std::filesystem::path& filePath)
    : _impl{std::make_unique<TextureImpl>()} {
  _impl->_sdlTexture =
      IMG_LoadTexture(renderer.getImpl().getSdlRenderer(), filePath.string().c_str());
  if (_impl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, const void* location,
                 std::size_t size)
    : _impl{std::make_unique<TextureImpl>()} {
  if (location == nullptr || size == 0) {
    throw Exception("Invalid texture data: null pointer or zero size");
  }
  
  SDL_IOStream* ioStream = SDL_IOFromConstMem(location, size);
  if (ioStream == nullptr) {
    throw Exception("SDL_IOFromConstMem");
  }

  _impl->_sdlTexture =
      IMG_LoadTexture_IO(renderer.getImpl().getSdlRenderer(), ioStream, true);
  if (_impl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, void* location, std::size_t size)
    : _impl{std::make_unique<TextureImpl>()} {
  if (location == nullptr || size == 0) {
    throw Exception("Invalid texture data: null pointer or zero size");
  }
  
  SDL_IOStream* ioStream = SDL_IOFromMem(location, size);
  if (ioStream == nullptr) {
    throw Exception("SDL_IOFromMem failed");
  }

  _impl->_sdlTexture =
      IMG_LoadTexture_IO(renderer.getImpl().getSdlRenderer(), ioStream, true);
  if (_impl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, uint32_t width, uint32_t height, const uint32_t* pixels)
    : _impl{std::make_unique<TextureImpl>()} {
  _impl->_sdlTexture = SDL_CreateTexture(
      renderer.getImpl().getSdlRenderer(), SDL_PIXELFORMAT_RGBA8888, 
      SDL_TEXTUREACCESS_STATIC, static_cast<int>(width), static_cast<int>(height));
  
  if (_impl->_sdlTexture == nullptr) {
    throw Exception("SDL_CreateTexture");
  }

  if (pixels != nullptr) {
    const int pitch = static_cast<int>(width * sizeof(uint32_t));
    if (!SDL_UpdateTexture(_impl->_sdlTexture, nullptr, pixels, pitch)) {
      SDL_DestroyTexture(_impl->_sdlTexture);
      throw Exception("SDL_UpdateTexture");
    }
  }
}

Texture::Texture(const Renderer& renderer, const Surface& surface) : _impl(std::make_unique<TextureImpl>()) {
  _impl->_sdlTexture = SDL_CreateTextureFromSurface(renderer.getImpl().getSdlRenderer(), surface.getImpl().getSdlSurface());
}

Texture::Texture(Texture&& other) noexcept
    : _impl{std::move(other._impl)} {}

Texture::~Texture() {
  if (_impl->_sdlTexture != nullptr) {
    SDL_DestroyTexture(_impl->_sdlTexture);
  }
}

auto Texture::operator=(Texture&& other) noexcept -> Texture& {
  if (&other == this) {
    return *this;
  }
  if (_impl->_sdlTexture != nullptr) {
    SDL_DestroyTexture(_impl->_sdlTexture);
  }
  _impl = std::move(other._impl);
  return *this;
}

void Texture::setTextureBlendMode(const BlendMode& blendMode) {
  bool returnValue = SDL_SetTextureBlendMode(_impl->_sdlTexture,
                                             sdlBlendModeMap[blendMode]);
  if (!returnValue) {
    throw Exception("SDL_SetTextureBlendMode");
  }
}

auto Texture::getSize() -> Rectangle<float> {
  float x;
  float y;
  auto success = SDL_GetTextureSize(_impl->_sdlTexture, &x, &y);

  if(!success) {
    throw Exception("SDL_GetTextureSize");
  }

  return { 0, 0, x, y };
}

auto Texture::getImpl() const -> const TextureImpl& {
  return *_impl;
}

}  // namespace sdl
