#ifndef SDL_SURFACE_H
#define SDL_SURFACE_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "color.h"

#include "sdlpp_export.h"

namespace sdlpp {

class SurfaceImpl;
class Texture;

/**
 * @brief CPU-accessible pixel buffer for software rendering and image manipulation
 *
 * Surface represents image data stored in system RAM that can be directly
 * accessed and modified pixel by pixel. Unlike Texture, surfaces allow
 * read/write access to individual pixels and support various image processing
 * operations. They are ideal for procedural generation, image editing, and
 * preparation of data before conversion to hardware-accelerated textures.
 *
 * Surfaces use RGBA format with 8 bits per color component. All pixel
 * coordinates use (0,0) as the top-left corner. Surface operations are
 * performed entirely on the CPU, making them slower than GPU-based texture
 * operations but more flexible for complex image manipulation.
 *
 * Common workflow: Load/Create → Manipulate pixels → Convert to Texture → Render
 *
 * @note Surfaces cannot be copied, only moved for performance reasons
 * @note All pixel operations are bounds-checked and may throw on invalid coordinates
 * @note Large surfaces consume significant RAM and processing time
 * @see Texture for hardware-accelerated rendering
 * @see Color for pixel color representation
 *
 * Example usage:
 * @code
 * // Create and manipulate surface
 * sdlpp::Surface surface(256, 256);
 * surface.fill(sdlpp::NamedColor::kBlue);
 * surface.setPixel(128, 128, sdlpp::NamedColor::kWhite);
 * 
 * // Convert to texture for fast rendering
 * sdlpp::Texture texture(renderer, surface);
 * renderer.copy(texture);
 * @endcode
 */
class SDLPP_EXPORT Surface {
  friend SurfaceImpl;
 public:
  /**
   * @brief Create a surface by loading an image file
   * @param filePath Path to the image file to load
   * @throws sdlpp::Exception if file loading fails or format is unsupported
   *
   * Loads an image file and creates a surface containing the pixel data.
   * Supports common formats like PNG, JPG, BMP, and others depending on
   * SDL_image capabilities. The loaded image is automatically converted
   * to RGBA format for consistent pixel access.
   *
   * @note File format support depends on SDL_image configuration
   * @note Large images may consume significant memory
   * @note The file is read immediately and can be deleted after construction
   */
  explicit Surface(const std::filesystem::path& filePath);

  /**
   * @brief Create a blank surface with specified dimensions
   * @param width Width of the surface in pixels (must be > 0)
   * @param height Height of the surface in pixels (must be > 0)
   * @throws sdlpp::Exception if surface creation fails or dimensions are invalid
   *
   * Creates an empty surface filled with transparent black pixels.
   * The surface is immediately ready for pixel manipulation operations.
   * Memory usage will be width * height * 4 bytes.
   *
   * @note All pixels are initialized to transparent black (RGBA 0,0,0,0)
   * @note Large dimensions may fail due to memory constraints
   */
  Surface(uint32_t width, uint32_t height);

  /**
   * @brief Create a surface from raw RGBA pixel data
   * @param width Width of the surface in pixels
   * @param height Height of the surface in pixels
   * @param pixels Vector containing RGBA pixel data (4 bytes per pixel)
   * @throws sdlpp::Exception if data size doesn't match dimensions or surface creation fails
   *
   * Creates a surface from existing pixel data in RGBA format. The pixel
   * vector must contain exactly width * height * 4 bytes, with each pixel
   * represented as red, green, blue, alpha (0-255 each).
   *
   * @note The pixel data is copied, so the source vector can be safely destroyed
   * @note Pixel data size must exactly match width * height * 4 bytes
   * @note Pixels are stored row by row from top-left to bottom-right
   */
  Surface(uint32_t width, uint32_t height, const std::vector<uint8_t>& pixels);

  /**
   * @brief Move constructor - transfers ownership of the surface data
   * @param other The surface to move from (will be left in valid but unspecified state)
   * @note The moved-from surface should not be used except for destruction
   */
  Surface(Surface&& other) noexcept;
  
  /**
   * @brief Copy constructor is explicitly deleted
   * @note Surfaces cannot be copied due to potentially large memory usage
   */
  Surface(const Surface& other) = delete;
  
  /**
   * @brief Destructor - automatically frees the surface memory
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   */
  ~Surface() noexcept;

  /**
   * @brief Move assignment operator - transfers ownership of the surface data
   * @param other The surface to move from (will be left in valid but unspecified state)
   * @return Reference to this surface
   * @note The moved-from surface should not be used except for destruction
   */
  auto operator=(Surface&& other) noexcept -> Surface&;
  
  /**
   * @brief Copy assignment is explicitly deleted
   * @note Surfaces cannot be copied due to potentially large memory usage
   */
  auto operator=(const Surface& other) -> Surface& = delete;

  /**
   * @brief Get the width of the surface in pixels
   * @return The width of the surface in pixels
   * @note This is always the width specified at construction time
   * @note This is a fast operation that returns cached data
   */
  [[nodiscard]] auto getWidth() const -> uint32_t;

  /**
   * @brief Get the height of the surface in pixels
   * @return The height of the surface in pixels
   * @note This is always the height specified at construction time
   * @note This is a fast operation that returns cached data
   */
  [[nodiscard]] auto getHeight() const -> uint32_t;

  /**
   * @brief Get a copy of all pixel data in RGBA format
   * @return Vector containing all pixel data (4 bytes per pixel)
   *
   * Returns a complete copy of the surface's pixel data. The returned
   * vector contains width * height * 4 bytes in RGBA format, organized
   * row by row from top-left to bottom-right.
   *
   * @warning This operation copies potentially large amounts of data
   * @note The returned data can be modified without affecting the surface
   * @note Consider getPixel() for accessing individual pixels
   */
  [[nodiscard]] auto getPixels() const -> std::vector<uint8_t>;

  /**
   * @brief Replace all pixel data with new RGBA values
   * @param pixels Vector containing new pixel data (4 bytes per pixel)
   * @throws sdlpp::Exception if data size doesn't match surface dimensions
   *
   * Replaces the entire surface contents with new pixel data. The pixel
   * vector must contain exactly width * height * 4 bytes in RGBA format.
   *
   * @note The existing pixel data is completely replaced
   * @note Pixel data size must exactly match getWidth() * getHeight() * 4
   * @warning This operation may be expensive for large surfaces
   */
  void setPixels(const std::vector<uint8_t>& pixels);

  /**
   * @brief Get the color of a specific pixel
   * @param x X coordinate of the pixel (0 to getWidth()-1)
   * @param y Y coordinate of the pixel (0 to getHeight()-1)
   * @return The color of the pixel at the specified coordinates
   * @throws sdlpp::Exception if coordinates are outside the surface bounds
   *
   * Retrieves the color of a single pixel. Coordinates are validated
   * against surface bounds and will throw if invalid.
   *
   * @note Coordinates use (0,0) as top-left corner
   * @note This is relatively fast but avoid in tight loops for large areas
   */
  [[nodiscard]] auto getPixel(uint32_t x, uint32_t y) const -> Color;

  /**
   * @brief Set the color of a specific pixel
   * @param x X coordinate of the pixel (0 to getWidth()-1)
   * @param y Y coordinate of the pixel (0 to getHeight()-1)
   * @param color The color to set for this pixel
   * @throws sdlpp::Exception if coordinates are outside the surface bounds
   *
   * Sets the color of a single pixel. Coordinates are validated against
   * surface bounds and will throw if invalid.
   *
   * @note Coordinates use (0,0) as top-left corner
   * @note This is relatively fast but avoid in tight loops for large areas
   * @note Consider fillRect() for setting multiple pixels to the same color
   */
  void setPixel(uint32_t x, uint32_t y, const Color& color);

  /**
   * @brief Fill the entire surface with a solid color
   * @param color The color to fill the surface with
   *
   * Sets every pixel in the surface to the specified color. This is
   * the fastest way to clear or initialize a surface to a uniform color.
   *
   * @note This operation affects every pixel in the surface
   * @note Much faster than setting individual pixels
   */
  void fill(const Color& color);

  /**
   * @brief Fill a rectangular region with a solid color
   * @param x X coordinate of the top-left corner of the rectangle
   * @param y Y coordinate of the top-left corner of the rectangle
   * @param width Width of the rectangle in pixels
   * @param height Height of the rectangle in pixels
   * @param color The color to fill the rectangle with
   * @throws sdlpp::Exception if the rectangle extends outside the surface bounds
   *
   * Fills a rectangular area with a solid color. The rectangle is clipped
   * to the surface bounds and will throw if completely outside.
   *
   * @note Rectangle coordinates use (0,0) as top-left corner of surface
   * @note Much faster than setting individual pixels
   * @note Rectangle must be at least partially within surface bounds
   */
  void fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color& color);

  /**
   * @brief Copy pixels from another surface to this surface
   * @param source The surface to copy from
   * @param destX X coordinate on this surface where copying begins (default: 0)
   * @param destY Y coordinate on this surface where copying begins (default: 0)
   * @throws sdlpp::Exception if the operation would extend outside surface bounds
   *
   * Copies the entire source surface to this surface at the specified
   * destination coordinates. The source surface size determines how much
   * data is copied.
   *
   * @note Source coordinates start at (0,0) of the source surface
   * @note Destination coordinates are clipped to this surface's bounds
   * @note Alpha blending is performed during the copy operation
   */
  void blit(const Surface& source, uint32_t destX = 0, uint32_t destY = 0);

  /**
   * @brief Copy a rectangular region from another surface
   * @param source The surface to copy from
   * @param srcX X coordinate on source surface to start copying from
   * @param srcY Y coordinate on source surface to start copying from
   * @param srcWidth Width of the region to copy from source
   * @param srcHeight Height of the region to copy from source
   * @param destX X coordinate on this surface where copying begins
   * @param destY Y coordinate on this surface where copying begins
   * @throws sdlpp::Exception if source or destination rectangles are invalid
   *
   * Copies a rectangular region from the source surface to this surface.
   * Both source and destination rectangles are validated against their
   * respective surface bounds.
   *
   * @note All coordinates are validated and will throw if out of bounds
   * @note Alpha blending is performed during the copy operation
   * @note This allows copying sub-regions and repositioning content
   */
  void blit(const Surface& source, uint32_t srcX, uint32_t srcY, 
            uint32_t srcWidth, uint32_t srcHeight, 
            uint32_t destX, uint32_t destY);

  /**
   * @brief Save the surface as an image file
   * @param filePath Path where the image file will be created
   * @throws sdlpp::Exception if file writing fails or format is unsupported
   *
   * Saves the surface pixel data to an image file. The output format is
   * determined by the file extension. Supports formats like PNG, BMP, and
   * others depending on SDL_image capabilities.
   *
   * @note PNG format recommended for lossless quality with alpha support
   * @note Directory path must exist before saving
   * @note Existing files will be overwritten without warning
   */
  void save(const std::filesystem::path& filePath) const;

  /**
   * @brief Get controlled access to implementation details
   * @return Const reference to the surface implementation
   * @note This method is used internally by related classes like Texture
   */
  [[nodiscard]] auto getImpl() const -> const SurfaceImpl&;

 private:
  Surface();
  std::unique_ptr<SurfaceImpl> _impl;
};

}  // namespace sdlpp

#endif
