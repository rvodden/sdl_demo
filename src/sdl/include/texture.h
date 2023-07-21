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
    //! @brief creates a texture from a file on disk.
    Texture(const Renderer& renderer, std::filesystem::path filePath);

    //! @brief creates a file from data already held in cpu memory.
    Texture(const Renderer& renderer, const void* location, std::size_t size);
    Texture(const Renderer& renderer, void* location, std::size_t size);

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
