#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

#include "sdl/color.h"
#include "sdl/rectangle.h"
#include "sdl_export.h"
#include "sdl/texture.h"

namespace sdl {

class Texture;
class Window;
class RendererImpl;

/**
 * @brief Hardware-accelerated 2D rendering context for drawing graphics
 *
 * Renderer provides a high-performance 2D rendering context that can draw
 * textures, primitives, and perform screen operations. It is associated with
 * a specific Window and manages the graphics state including draw color,
 * blend modes, and render targets.
 *
 * The renderer uses hardware acceleration when available and falls back to
 * software rendering if necessary. All rendering operations are batched and
 * only become visible when present() is called, following a typical
 * double-buffering pattern.
 *
 * Typical rendering loop:
 * 1. clear() - Clear the back buffer
 * 2. copy() - Draw textures and sprites  
 * 3. present() - Display the completed frame
 *
 * @note Renderers cannot be copied, only moved for performance reasons
 * @note The renderer must outlive any Textures created from it
 * @see Window for the display target
 * @see Texture for renderable image data
 * @see Color for draw color specification
 *
 * Example usage:
 * @code
 * sdl::Window window("Game", 800, 600, 0);
 * sdl::Renderer renderer(window);
 * 
 * // Set background color
 * renderer.setDrawColour(sdl::NamedColor::kBlack);
 * 
 * // Render loop
 * while (running) {
 *     renderer.clear();
 *     renderer.copy(sprite_texture);
 *     renderer.present();
 * }
 * @endcode
 */
class SDL_EXPORT Renderer {

 public:
  using RendererFlag = uint8_t;

  /**
   * @brief Create a rendering context for the specified window
   * @param window The window to create the renderer for (must remain valid)
   * @param flags Optional renderer flags (default: hardware accelerated with VSync)
   * @throws sdl::Exception if renderer creation fails
   *
   * Creates a hardware-accelerated renderer when possible, falling back
   * to software rendering if hardware acceleration is unavailable. The
   * renderer will target the entire window surface.
   *
   * @note The window must remain valid for the lifetime of this renderer
   * @note Automatic driver selection usually chooses the best available option
   */
  Renderer(Window& window, uint32_t flags = kAccelerated | kPresentVSync);
  
  /**
   * @brief Move constructor - transfers ownership of the rendering context
   * @param other The renderer to move from (will be left in valid but unspecified state)
   * @note The moved-from renderer should not be used except for destruction
   */
  Renderer(Renderer&& other) noexcept;
  
  /**
   * @brief Copy constructor is explicitly deleted
   * @note Renderers cannot be copied due to the underlying graphics context
   */
  Renderer(const Renderer& other) = delete;
  
  /**
   * @brief Destructor - automatically destroys the rendering context
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   * @note All associated textures must be destroyed before the renderer
   */
  ~Renderer() noexcept;

  /**
   * @brief Move assignment operator - transfers ownership of the rendering context
   * @param other The renderer to move from (will be left in valid but unspecified state)
   * @return Reference to this renderer
   * @note The moved-from renderer should not be used except for destruction
   */
  auto operator=(Renderer&& other) noexcept -> Renderer&;
  
  /**
   * @brief Copy assignment is explicitly deleted
   * @note Renderers cannot be copied due to the underlying graphics context
   */
  auto operator=(const Renderer& other) -> Renderer& = delete;

  /**
   * @brief Set the color used for drawing operations and clearing
   * @param color The color to use for subsequent drawing operations
   *
   * Sets the draw color used by clear() and future primitive drawing
   * operations. The color is applied immediately and persists until
   * changed. Alpha component is used for blending operations.
   *
   * @note This is a fast operation that only updates internal state
   * @see Color for color specification
   * @see NamedColor for common color constants
   */
  void setDrawColour(const Color& color);

  /**
   * @brief Draw a texture to the entire render target
   * @param texture The texture to draw (must be created from this renderer)
   *
   * Copies the entire texture to the render target, scaling to fit the
   * full window dimensions. This is the fastest way to draw a texture
   * that covers the entire screen.
   *
   * @note The texture must have been created from this renderer
   * @note Drawing is not immediately visible until present() is called
   * @see copy(const Texture&, const Rectangle<float>&, const Rectangle<float>&) for partial copies
   */
  void copy(const Texture& texture);

  /**
   * @brief Draw a texture to a specific region
   * @param texture The texture to draw from (must be created from this renderer)
   * @param destination The screen rectangle to draw to
   *
   * Copies the texture to a specific location
   * on the render target.
   *
   * @note The texture must have been created from this renderer
   * @note Destination coordinates are in screen space (0,0 = top-left of window)
   * @note Drawing is not immediately visible until present() is called
   */
  void copy(const Texture& texture, const Rectangle<float>& destination);
  
            /**
   * @brief Draw a portion of a texture to a specific region
   * @param texture The texture to draw from (must be created from this renderer)
   * @param source The rectangle within the texture to copy from
   * @param destination The screen rectangle to draw to
   *
   * Copies a rectangular region from the texture to a specific location
   * on the render target. Both source and destination can be any size,
   * allowing for scaling and cropping in a single operation.
   *
   * @note The texture must have been created from this renderer
   * @note Source coordinates are in texture space (0,0 = top-left of texture)
   * @note Destination coordinates are in screen space (0,0 = top-left of window)
   * @note Drawing is not immediately visible until present() is called
   */
  void copy(const Texture& texture, const Rectangle<float>& source,
            const Rectangle<float>& destination);

  /**
   * @brief Clear the entire render target with the current draw color
   *
   * Fills the entire render target with the color set by setDrawColour().
   * This is typically the first operation in a render loop to provide a
   * clean background for drawing operations.
   *
   * @note Uses the color set by setDrawColour()
   * @note Changes are not visible until present() is called
   * @note This is usually the fastest way to fill the screen with a solid color
   */
  void clear() const;

  /**
   * @brief Display all rendering operations performed since the last present()
   *
   * Swaps the back buffer to the front buffer, making all rendering operations
   * since the last present() call visible on screen. This implements double
   * buffering for smooth animation without flickering.
   *
   * @note This may block for vertical sync depending on system settings
   * @note Call this exactly once per frame for optimal performance
   * @note All rendering between present() calls is batched for efficiency
   */
  void present() const;

  /**
   * @brief Read pixel data from a rectangular region of the render target
   * @param x X coordinate of the top-left corner to read from
   * @param y Y coordinate of the top-left corner to read from
   * @param width Width of the rectangle to read in pixels
   * @param height Height of the rectangle to read in pixels
   * @return Vector of pixel data in RGBA format (4 bytes per pixel)
   * @throws sdl::Exception if the read operation fails
   *
   * Reads raw pixel data from the current render target. The returned data
   * is in RGBA format with 4 bytes per pixel (red, green, blue, alpha).
   * The data is ordered row by row from top to bottom, left to right.
   *
   * @warning This is a very expensive operation that may cause GPU stalls
   * @note The returned vector has size width * height * 4 bytes
   * @note Pixel data reflects the current state of the render target
   * @note Consider caching results if reading the same region repeatedly
   */
  [[nodiscard]] auto readPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const -> std::vector<uint8_t>;

  [[nodiscard]] auto getOutputSize() -> Rectangle<int>;

  void setScale(float xScale, float yScale);

  void drawLine(float x1, float y1, float x2, float y2);

  void fillRect(const Rectangle<float>& rect);

  /**
   * @brief Get controlled access to implementation details
   * @return Const reference to the renderer implementation
   * @note This method is used internally by related classes like Texture
   */
  [[nodiscard]] auto getImpl() const -> const RendererImpl&;

  /** @brief Software-only rendering (slowest, most compatible) */
  static constexpr RendererFlag kSoftware = 0;
  /** @brief Hardware-accelerated rendering (fastest when available) */
  static constexpr RendererFlag kAccelerated = 1;
  /** @brief Enable vertical sync to prevent tearing */
  static constexpr RendererFlag kPresentVSync = 2;
  /** @brief Support rendering to textures as render targets */
  static constexpr RendererFlag kTargetTexture = 3;

 private:
  std::unique_ptr<RendererImpl> _impl;
};

}  // namespace sdl

#endif
