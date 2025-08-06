#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "exception.h"
#include "renderer_impl.h"
#include "texture_impl.h"
#include "texture.h"

namespace sdl {

Texture::Texture(const Renderer& renderer, std::filesystem::path filePath) : _textureImpl { std::make_unique<TextureImpl>() } {
  _textureImpl->_sdlTexture = IMG_LoadTexture(renderer._rendererImpl->_sdlRenderer, filePath.c_str());
  if  (_textureImpl->_sdlTexture == nullptr ) throw Exception("IMG_LoadTexture");
}

Texture::Texture(const Renderer &renderer, const void *location, std::size_t size) : _textureImpl { std::make_unique<TextureImpl>() } {
  SDL_RWops* rwOps = SDL_RWFromConstMem(location, static_cast<int>(size));
  
  _textureImpl->_sdlTexture = IMG_LoadTexture_RW(renderer._rendererImpl->_sdlRenderer, rwOps, 1);
  if  (_textureImpl->_sdlTexture == nullptr ) throw Exception("IMG_LoadTexture");
}

Texture::Texture(const Renderer &renderer, void *location, std::size_t size) : _textureImpl { std::make_unique<TextureImpl>() } {
  SDL_RWops* rwOps = SDL_RWFromMem(location, static_cast<int>(size));
  
  _textureImpl->_sdlTexture = IMG_LoadTexture_RW(renderer._rendererImpl->_sdlRenderer, rwOps, 1);
  if  (_textureImpl->_sdlTexture == nullptr ) throw Exception("IMG_LoadTexture");
}

Texture::Texture(Texture &&other) noexcept : _textureImpl { std::move(other._textureImpl) } { }

Texture::~Texture() {
  if(_textureImpl->_sdlTexture != nullptr) SDL_DestroyTexture(_textureImpl->_sdlTexture);
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if(&other == this) return *this;
  if(_textureImpl->_sdlTexture != nullptr) SDL_DestroyTexture(_textureImpl->_sdlTexture);
  _textureImpl = std::move(other._textureImpl);
  return *this;
}

void Texture::setTextureBlendMode(const BlendMode &blendMode) {
  int returnValue = SDL_SetTextureBlendMode(_textureImpl->_sdlTexture, sdlBlendModeMap[blendMode]);
  if( returnValue < 0 ) throw Exception("SDL_SetTextureBlendMode");
}

}
