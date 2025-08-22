#ifndef SDL_SURFACE_H
#define SDL_SURFACE_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "color.h"
#include "sdl_export.h"

namespace sdlpp {

class SurfaceImpl;

//! @brief A pixel buffer for software rendering and image manipulation
class SDL_EXPORT Surface {
 public:
  //! @brief Create a surface from an image file
  explicit Surface(const std::filesystem::path& filePath);

  //! @brief Create a surface with specified dimensions
  Surface(uint32_t width, uint32_t height);

  //! @brief Create a surface from raw pixel data
  Surface(uint32_t width, uint32_t height, const std::vector<uint8_t>& pixels);

  //! @brief move constructor
  Surface(Surface&& other) noexcept;
  //! @brief Surface cannot be copied
  Surface(const Surface& other) = delete;
  //! @brief destructor
  ~Surface() noexcept;

  //! @brief move assignment operator
  auto operator=(Surface&& other) noexcept -> Surface&;
  //! @brief Surface cannot be copied
  auto operator=(const Surface& other) -> Surface& = delete;

  //! @brief Get the width of the surface in pixels
  [[nodiscard]] auto getWidth() const -> uint32_t;

  //! @brief Get the height of the surface in pixels
  [[nodiscard]] auto getHeight() const -> uint32_t;

  //! @brief Get pixel data as RGBA values
  [[nodiscard]] auto getPixels() const -> std::vector<uint8_t>;

  //! @brief Set pixel data from RGBA values
  void setPixels(const std::vector<uint8_t>& pixels);

  //! @brief Get the color of a specific pixel
  [[nodiscard]] auto getPixel(uint32_t x, uint32_t y) const -> Color;

  //! @brief Set the color of a specific pixel
  void setPixel(uint32_t x, uint32_t y, const Color& color);

  //! @brief Fill the entire surface with a color
  void fill(const Color& color);

  //! @brief Fill a rectangular region with a color
  void fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color& color);

  //! @brief Copy from another surface
  void blit(const Surface& source, uint32_t destX = 0, uint32_t destY = 0);

  //! @brief Copy a region from another surface
  void blit(const Surface& source, uint32_t srcX, uint32_t srcY, 
            uint32_t srcWidth, uint32_t srcHeight, 
            uint32_t destX, uint32_t destY);

  //! @brief Save the surface to an image file
  void save(const std::filesystem::path& filePath) const;

 private:
  std::unique_ptr<SurfaceImpl> _surfaceImpl;
};

}  // namespace sdlpp

#endif