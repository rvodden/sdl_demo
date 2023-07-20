#ifndef __SDL_TEXTURE_H__
#define __SDL_TEXTURE_H__

#include <filesystem>

#include "renderer.h"

namespace sdl {

class Renderer;
class TextureImpl;

//! @brief Image stored in the graphics card memory that can be used for fast drawing
class Texture {
  friend Renderer;
  public:
    Texture(const Renderer& renderer, std::filesystem::path filePath);
    Texture(Texture& other) = delete;
    Texture(Texture&& other) noexcept;
    ~Texture();

    Texture& operator=(Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept;

    //TODO: implement other constructors and operators

  private:
    std::unique_ptr<TextureImpl> _textureImpl;
};


}

#endif
