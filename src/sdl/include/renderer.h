#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include <memory>
#include <optional>
#include <unordered_set>

#include "color.h"
#include "float_rectangle.h"
#include "texture.h"

namespace sdl {

class FloatRectangle;
class Texture;
class Window;
class RendererImpl;

//! @brief A 2D Rendering Context
class Renderer {
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

  static constexpr RendererFlag kSoftware = 0;
  static constexpr RendererFlag kAccelerated = 1;
  static constexpr RendererFlag kPresentVSync = 2;
  static constexpr RendererFlag kTargetTexture = 3;

 private:
  std::unique_ptr<RendererImpl> _rendererImpl;
};

}  // namespace sdl

#endif
