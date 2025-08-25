#ifndef SDL_TEXTURE_H
#define SDL_TEXTURE_H

#include <filesystem>

#include "renderer.h"
#include "surface.h"
#include "sdlpp_export.h"

namespace sdlpp {

class Renderer;
class TextureImpl;

/**
 * @brief Hardware-accelerated image stored in graphics memory for fast rendering
 *
 * Texture represents image data optimized for fast rendering operations.
 * Textures are stored in graphics card memory (VRAM) and can be drawn
 * efficiently by the GPU using hardware acceleration. They are the primary
 * way to display images, sprites, and graphical content.
 *
 * Textures are associated with a specific Renderer and cannot be shared
 * between different rendering contexts. They support various blend modes
 * for advanced rendering effects like transparency and additive blending.
 *
 * Once created, texture pixel data cannot be modified directly. For dynamic
 * content, create a new texture or use render-to-texture techniques.
 *
 * @note Textures cannot be copied, only moved for performance reasons
 * @note The associated renderer must outlive all textures created from it
 * @note Texture coordinates use (0,0) as top-left corner
 * @see Renderer for drawing textures to the screen
 * @see Surface for CPU-based image manipulation before creating textures
 * @see BlendMode constants for rendering effects
 *
 * Example usage:
 * @code
 * sdlpp::Renderer renderer(window);
 * 
 * // Load from file
 * sdlpp::Texture sprite(renderer, "player.png");
 * 
 * // Configure blending
 * sprite.setTextureBlendMode(sdlpp::Texture::kBlend);
 * 
 * // Draw to screen
 * renderer.copy(sprite);
 * renderer.present();
 * @endcode
 */
class SDLPP_EXPORT Texture {
  friend Renderer;

 public:
  using BlendMode = uint8_t;

  /**
   * @brief Create a texture by loading an image file
   * @param renderer The renderer that will own this texture (must remain valid)
   * @param filePath Path to the image file to load
   * @throws sdlpp::Exception if file loading or texture creation fails
   *
   * Loads an image file and creates a hardware-accelerated texture.
   * Supports common formats like PNG, JPG, BMP, and others depending
   * on SDL_image capabilities. The image is automatically converted
   * to the optimal format for the graphics hardware.
   *
   * @note The renderer must remain valid for the lifetime of this texture
   * @note File format support depends on SDL_image configuration
   * @note Large images may consume significant VRAM
   */
  Texture(const Renderer& renderer, const std::filesystem::path& filePath);

  /**
   * @brief Create a texture from compressed image data in memory
   * @param renderer The renderer that will own this texture (must remain valid)
   * @param location Pointer to compressed image data (PNG, JPG, etc.)
   * @param size Size of the compressed image data in bytes
   * @throws sdlpp::Exception if data parsing or texture creation fails
   *
   * Creates a texture from compressed image data already loaded in memory.
   * The data should be in a standard format like PNG or JPG. This is useful
   * for embedded resources or network-loaded images.
   *
   * @note The data pointer can be freed after the constructor returns
   * @note The renderer must remain valid for the lifetime of this texture
   */
  Texture(const Renderer& renderer, const void* location, std::size_t size);
  
  /**
   * @brief Create a texture from compressed image data in memory (non-const version)
   * @param renderer The renderer that will own this texture (must remain valid)
   * @param location Pointer to compressed image data (PNG, JPG, etc.)
   * @param size Size of the compressed image data in bytes
   * @throws sdlpp::Exception if data parsing or texture creation fails
   *
   * @see Texture(const Renderer&, const void*, std::size_t) for details
   */
  Texture(const Renderer& renderer, void* location, std::size_t size);

  /**
   * @brief Create a texture from raw RGBA pixel data
   * @param renderer The renderer that will own this texture (must remain valid)
   * @param width Width of the image in pixels
   * @param height Height of the image in pixels
   * @param pixels Pointer to RGBA pixel data (4 bytes per pixel)
   * @throws sdlpp::Exception if texture creation fails
   *
   * Creates a texture from raw pixel data in RGBA format. The pixel data
   * should contain width * height pixels, with 4 bytes per pixel in
   * red-green-blue-alpha order.
   *
   * @note The pixel data can be freed after the constructor returns
   * @note Pixel format is assumed to be RGBA with 8 bits per component
   * @note The renderer must remain valid for the lifetime of this texture
   */
  Texture(const Renderer& renderer, uint32_t width, uint32_t height, const uint32_t* pixels);

  /**
   * @brief Create a texture from a Surface
   * @param renderer The renderer that will own this texture (must remain valid)
   * @param surface The surface containing the image data to convert
   * @throws sdlpp::Exception if texture creation fails
   *
   * Converts a CPU-based Surface to a hardware-accelerated Texture.
   * This is the preferred way to create textures from programmatically
   * generated or modified image data.
   *
   * @note The surface can be destroyed after the constructor returns
   * @note The renderer must remain valid for the lifetime of this texture
   * @see Surface for CPU-based image manipulation
   */
  Texture(const Renderer& renderer, const Surface& surface);

  /**
   * @brief Copy constructor is explicitly deleted
   * @note Textures cannot be copied due to the underlying graphics resource
   */
  Texture(Texture& other) = delete;
  
  /**
   * @brief Move constructor - transfers ownership of the texture
   * @param other The texture to move from (will be left in valid but unspecified state)
   * @note The moved-from texture should not be used except for destruction
   */
  Texture(Texture&& other) noexcept;
  
  /**
   * @brief Destructor - automatically destroys the graphics texture
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   */
  ~Texture();

  /**
   * @brief Copy assignment is explicitly deleted
   * @note Textures cannot be copied due to the underlying graphics resource
   */
  auto operator=(Texture& other) -> Texture& = delete;
  
  /**
   * @brief Move assignment operator - transfers ownership of the texture
   * @param other The texture to move from (will be left in valid but unspecified state)
   * @return Reference to this texture
   * @note The moved-from texture should not be used except for destruction
   */
  auto operator=(Texture&& other) noexcept -> Texture&;

  /**
   * @brief Set the blend mode used when drawing this texture
   * @param blendMode The blending mode to use (see BlendMode constants)
   *
   * Controls how this texture is blended with the destination when drawn.
   * The blend mode affects how alpha values and color channels interact
   * with existing content on the render target.
   *
   * @note The blend mode persists until changed
   * @note Default blend mode is typically kBlend for alpha blending
   * @see BlendMode constants for available options
   */
  void setTextureBlendMode(const BlendMode& blendMode);
  
  /**
   * @brief Get the current blend mode for this texture
   * @return The current blend mode
   * @note This is a fast operation that returns cached state
   */
  [[nodiscard]] auto getTextureBlendMode() -> BlendMode;

  /** @brief No blending - source pixels replace destination pixels completely */
  static constexpr BlendMode kNone = 0;
  /** @brief Alpha blending - standard transparency using alpha channel */
  static constexpr BlendMode kBlend = 1;  
  /** @brief Additive blending - colors are added together (brightening effect) */
  static constexpr BlendMode kAdd = 2;
  /** @brief Modulate blending - colors are multiplied together (darkening effect) */
  static constexpr BlendMode kMod = 3;
  /** @brief Multiply blending - similar to modulate but with different alpha handling */
  static constexpr BlendMode kMul = 4;

 private:
  std::unique_ptr<TextureImpl> _textureImpl;
};

}  // namespace sdlpp

#endif
