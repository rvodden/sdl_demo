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

Texture::Texture(Texture &&other) : _textureImpl { std::move(other._textureImpl) } { }


Texture::~Texture() {
  if(_textureImpl->_sdlTexture != nullptr) SDL_DestroyTexture(_textureImpl->_sdlTexture);
}

Texture &Texture::operator=(Texture &&other) {
  if(&other == this) return *this;
  if(_textureImpl->_sdlTexture != nullptr) SDL_DestroyTexture(_textureImpl->_sdlTexture);
  _textureImpl = std::move(other._textureImpl);
  return *this;
}

}
