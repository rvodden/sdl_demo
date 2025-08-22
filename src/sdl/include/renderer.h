#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

#include "color.h"
#include "float_rectangle.h"
#include "sdl_export.h"
#include "texture.h"

namespace sdlpp {

class FloatRectangle;
class Texture;
class Window;
class RendererImpl;

//! @brief A 2D Rendering Context
class SDL_EXPORT Renderer {
  friend Texture;

 public:
  using RendererFlag = uint8_t;

  /**
   * @brief Construct a renderer associated with the provided window
   */
  Renderer(Window& window, const char* name = nullptr);
  //! @brief move constructor
  Renderer(Renderer&& other) noexcept;
  //! @brief Renderer cannot be copied
  Renderer(const Renderer& other) = delete;
  //! @brief destructor
  ~Renderer() noexcept;

  //! @brief move assignment operator
  auto operator=(Renderer&& other) noexcept -> Renderer&;
  //! @brief Renderer cannot be copied
  auto operator=(const Renderer& other) -> Renderer& = delete;

  //! @brief Set the color used for drawing operations (Rect, Line and Clear).
  void setRenderDrawColour(const Color& color);

  //! @brief Copy a region of the texture to a region of the renderer.
  void copy(const Texture& texture);

  //! @brief Copy a region of the texture to a region of the renderer.
  void copy(const Texture& texture, const FloatRectangle& source,
            const FloatRectangle& destination);

  //! @brief Clear the renderer with the drawing color.
  void clear() const;

  //! @brief Update the screen with any rendering performed since the
  //! previous call.
  void present() const;

  //! @brief Read pixels from the current render target.
  //! @param x X coordinate of the rectangle to read
  //! @param y Y coordinate of the rectangle to read  
  //! @param width Width of the rectangle to read
  //! @param height Height of the rectangle to read
  //! @return Vector of pixel data in RGBA format
  [[nodiscard]] auto readPixels(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const -> std::vector<uint8_t>;

  static constexpr RendererFlag kSoftware = 0;
  static constexpr RendererFlag kAccelerated = 1;
  static constexpr RendererFlag kPresentVSync = 2;
  static constexpr RendererFlag kTargetTexture = 3;

 private:
  std::unique_ptr<RendererImpl> _rendererImpl;
};

}  // namespace sdlpp

#endif
