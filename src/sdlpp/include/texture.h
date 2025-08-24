#ifndef SDL_TEXTURE_H
#define SDL_TEXTURE_H

#include <filesystem>

#include "renderer.h"
#include "surface.h"
#include "sdlpp_export.h"

namespace sdlpp {

class Renderer;
class TextureImpl;

//! @brief Image stored in the graphics card memory that can be used for fast
//! drawing
class SDLPP_EXPORT Texture {
  friend Renderer;

 public:
  using BlendMode = uint8_t;

  //! @brief creates a texture from a file on disk.
  Texture(const Renderer& renderer, const std::filesystem::path& filePath);

  //! @brief creates a file from data already held in cpu memory.
  Texture(const Renderer& renderer, const void* location, std::size_t size);
  Texture(const Renderer& renderer, void* location, std::size_t size);

  //! @brief creates a texture from raw RGBA pixel data
  Texture(const Renderer& renderer, uint32_t width, uint32_t height, const uint32_t* pixels);

  //! @brief create a texture from a Surface
  Texture(const Renderer& renderer, const Surface& surface);

  Texture(Texture& other) = delete;
  Texture(Texture&& other) noexcept;
  ~Texture();

  auto operator=(Texture& other) -> Texture& = delete;
  auto operator=(Texture&& other) noexcept -> Texture&;

  void setTextureBlendMode(const BlendMode& blendMode);
  auto getTextureBlendMode() -> BlendMode;

  static constexpr BlendMode kNone = 0;
  static constexpr BlendMode kBlend = 1;
  static constexpr BlendMode kAdd = 2;
  static constexpr BlendMode kMod = 3;
  static constexpr BlendMode kMul = 4;

 private:
  std::unique_ptr<TextureImpl> _textureImpl;
};

}  // namespace sdlpp

#endif
