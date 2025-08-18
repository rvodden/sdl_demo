#include "texture.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "exception.h"
#include "renderer_impl.h"
#include "texture_impl.h"

namespace sdl {

Texture::Texture(const Renderer& renderer,
                 const std::filesystem::path& filePath)
    : _textureImpl{std::make_unique<TextureImpl>()} {
  _textureImpl->_sdlTexture =
      IMG_LoadTexture(renderer._rendererImpl->_sdlRenderer, filePath.c_str());
  if (_textureImpl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, const void* location,
                 std::size_t size)
    : _textureImpl{std::make_unique<TextureImpl>()} {
  SDL_IOStream* ioStream = SDL_IOFromConstMem(location, size);

  _textureImpl->_sdlTexture =
      IMG_LoadTexture_IO(renderer._rendererImpl->_sdlRenderer, ioStream, true);
  if (_textureImpl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, void* location, std::size_t size)
    : _textureImpl{std::make_unique<TextureImpl>()} {
  SDL_IOStream* ioStream = SDL_IOFromMem(location, size);

  _textureImpl->_sdlTexture =
      IMG_LoadTexture_IO(renderer._rendererImpl->_sdlRenderer, ioStream, true);
  if (_textureImpl->_sdlTexture == nullptr) {
    throw Exception("IMG_LoadTexture");
  }
}

Texture::Texture(const Renderer& renderer, uint32_t width, uint32_t height, const uint32_t* pixels)
    : _textureImpl{std::make_unique<TextureImpl>()} {
  _textureImpl->_sdlTexture = SDL_CreateTexture(
      renderer._rendererImpl->_sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
      SDL_TEXTUREACCESS_STATIC, static_cast<int>(width), static_cast<int>(height));
  
  if (_textureImpl->_sdlTexture == nullptr) {
    throw Exception("SDL_CreateTexture");
  }

  if (pixels != nullptr) {
    const int pitch = static_cast<int>(width * sizeof(uint32_t));
    if (!SDL_UpdateTexture(_textureImpl->_sdlTexture, nullptr, pixels, pitch)) {
      SDL_DestroyTexture(_textureImpl->_sdlTexture);
      throw Exception("SDL_UpdateTexture");
    }
  }
}

Texture::Texture(Texture&& other) noexcept
    : _textureImpl{std::move(other._textureImpl)} {}

Texture::~Texture() {
  if (_textureImpl->_sdlTexture != nullptr) {
    SDL_DestroyTexture(_textureImpl->_sdlTexture);
  }
}

auto Texture::operator=(Texture&& other) noexcept -> Texture& {
  if (&other == this) {
    return *this;
  }
  if (_textureImpl->_sdlTexture != nullptr) {
    SDL_DestroyTexture(_textureImpl->_sdlTexture);
  }
  _textureImpl = std::move(other._textureImpl);
  return *this;
}

void Texture::setTextureBlendMode(const BlendMode& blendMode) {
  bool returnValue = SDL_SetTextureBlendMode(_textureImpl->_sdlTexture,
                                             sdlBlendModeMap[blendMode]);
  if (!returnValue) {
    throw Exception("SDL_SetTextureBlendMode");
  }
}

}  // namespace sdl
