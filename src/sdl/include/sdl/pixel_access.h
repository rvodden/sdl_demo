#ifndef SDL_PIXEL_ACCESS_H
#define SDL_PIXEL_ACCESS_H

#include <cstddef>
#include <cstdint>
#include <span>

namespace sdl {

/**
 * @brief RAII guard providing zero-copy access to Surface pixel data
 *
 * PixelAccess provides direct, zero-copy access to the underlying pixel buffer
 * of a Surface. It follows the same RAII pattern as EventRegistration - it's
 * move-only and non-copyable to communicate that it represents borrowed access
 * to the Surface's memory.
 *
 * This class is designed for high-performance pixel manipulation where the
 * overhead of copying pixel data is unacceptable. The returned spans provide
 * direct access to the Surface's internal buffer.
 *
 * @warning The PixelAccess object must not outlive the Surface it was obtained from.
 *          Using the access after the Surface is destroyed or moved results in
 *          undefined behavior.
 *
 * @note This class is move-only to emphasize the borrowing relationship
 * @note For safety, prefer keeping PixelAccess objects short-lived
 *
 * Usage example:
 * @code
 * sdl::Surface surface(256, 256);
 * {
 *     auto pixels = surface.getPixels();
 *     // Direct access to pixel buffer - zero copy
 *     pixels[0] = 255;  // Set first byte
 *     auto span = pixels.span();
 *     std::fill(span.begin(), span.end(), 0);  // Clear all pixels
 * }
 * // PixelAccess destroyed here, but Surface still valid
 * @endcode
 */
class PixelAccess {
  friend class Surface;

 public:
  /**
   * @brief Move constructor - transfers access ownership
   * @param other The PixelAccess to move from
   */
  PixelAccess(PixelAccess&& other) noexcept = default;

  /**
   * @brief Move assignment operator - transfers access ownership
   * @param other The PixelAccess to move from
   * @return Reference to this PixelAccess
   */
  auto operator=(PixelAccess&& other) noexcept -> PixelAccess& = default;

  /**
   * @brief Copy constructor is explicitly deleted
   * @note PixelAccess is move-only to enforce clear ownership semantics
   */
  PixelAccess(const PixelAccess&) = delete;

  /**
   * @brief Copy assignment is explicitly deleted
   * @note PixelAccess is move-only to enforce clear ownership semantics
   */
  auto operator=(const PixelAccess&) -> PixelAccess& = delete;

  /**
   * @brief Default destructor
   */
  ~PixelAccess() = default;

  /**
   * @brief Get a pointer to the pixel data
   * @return Pointer to the first byte of pixel data
   */
  [[nodiscard]] auto data() -> uint8_t* { return _span.data(); }

  /**
   * @brief Get a const pointer to the pixel data
   * @return Const pointer to the first byte of pixel data
   */
  [[nodiscard]] auto data() const -> const uint8_t* { return _span.data(); }

  /**
   * @brief Get the size of the pixel buffer in bytes
   * @return Total number of bytes in the pixel buffer (width * height * 4)
   */
  [[nodiscard]] auto size() const -> size_t { return _span.size(); }

  /**
   * @brief Get a mutable span view of the pixel data
   * @return Span covering all pixel bytes
   */
  [[nodiscard]] auto span() -> std::span<uint8_t> { return _span; }

  /**
   * @brief Get a const span view of the pixel data
   * @return Const span covering all pixel bytes
   */
  [[nodiscard]] auto span() const -> std::span<const uint8_t> { return _span; }

  /**
   * @brief Access a specific byte in the pixel buffer
   * @param index Byte index (0 to size()-1)
   * @return Reference to the byte at the given index
   * @note No bounds checking - use span() for safe access with at()
   */
  [[nodiscard]] auto operator[](size_t index) -> uint8_t& { return _span[index]; }

  /**
   * @brief Access a specific byte in the pixel buffer (const)
   * @param index Byte index (0 to size()-1)
   * @return Const reference to the byte at the given index
   * @note No bounds checking - use span() for safe access with at()
   */
  [[nodiscard]] auto operator[](size_t index) const -> const uint8_t& { return _span[index]; }

  /**
   * @brief Get iterator to the beginning of the pixel data
   * @return Iterator to the first byte
   */
  [[nodiscard]] auto begin() -> uint8_t* { return _span.data(); }

  /**
   * @brief Get const iterator to the beginning of the pixel data
   * @return Const iterator to the first byte
   */
  [[nodiscard]] auto begin() const -> const uint8_t* { return _span.data(); }

  /**
   * @brief Get iterator to the end of the pixel data
   * @return Iterator past the last byte
   */
  [[nodiscard]] auto end() -> uint8_t* { return _span.data() + _span.size(); }

  /**
   * @brief Get const iterator to the end of the pixel data
   * @return Const iterator past the last byte
   */
  [[nodiscard]] auto end() const -> const uint8_t* { return _span.data() + _span.size(); }

  /**
   * @brief Check if the pixel access is empty/invalid
   * @return True if the span is empty, false otherwise
   */
  [[nodiscard]] auto empty() const -> bool { return _span.empty(); }

 private:
  /**
   * @brief Internal constructor used by Surface
   * @param span Span view of the pixel data
   */
  explicit PixelAccess(std::span<uint8_t> span) : _span(span) {}

  std::span<uint8_t> _span;
};

/**
 * @brief RAII guard providing read-only zero-copy access to Surface pixel data
 *
 * ConstPixelAccess is the const counterpart to PixelAccess. It provides
 * read-only direct access to the underlying pixel buffer of a Surface.
 *
 * @warning The ConstPixelAccess object must not outlive the Surface it was obtained from.
 *
 * @note This class is move-only to emphasize the borrowing relationship
 */
class ConstPixelAccess {
  friend class Surface;

 public:
  /**
   * @brief Move constructor - transfers access ownership
   * @param other The ConstPixelAccess to move from
   */
  ConstPixelAccess(ConstPixelAccess&& other) noexcept = default;

  /**
   * @brief Move assignment operator - transfers access ownership
   * @param other The ConstPixelAccess to move from
   * @return Reference to this ConstPixelAccess
   */
  auto operator=(ConstPixelAccess&& other) noexcept -> ConstPixelAccess& = default;

  /**
   * @brief Copy constructor is explicitly deleted
   */
  ConstPixelAccess(const ConstPixelAccess&) = delete;

  /**
   * @brief Copy assignment is explicitly deleted
   */
  auto operator=(const ConstPixelAccess&) -> ConstPixelAccess& = delete;

  /**
   * @brief Default destructor
   */
  ~ConstPixelAccess() = default;

  /**
   * @brief Get a const pointer to the pixel data
   * @return Const pointer to the first byte of pixel data
   */
  [[nodiscard]] auto data() const -> const uint8_t* { return _span.data(); }

  /**
   * @brief Get the size of the pixel buffer in bytes
   * @return Total number of bytes in the pixel buffer (width * height * 4)
   */
  [[nodiscard]] auto size() const -> size_t { return _span.size(); }

  /**
   * @brief Get a const span view of the pixel data
   * @return Const span covering all pixel bytes
   */
  [[nodiscard]] auto span() const -> std::span<const uint8_t> { return _span; }

  /**
   * @brief Access a specific byte in the pixel buffer (const)
   * @param index Byte index (0 to size()-1)
   * @return Const reference to the byte at the given index
   */
  [[nodiscard]] auto operator[](size_t index) const -> const uint8_t& { return _span[index]; }

  /**
   * @brief Get const iterator to the beginning of the pixel data
   * @return Const iterator to the first byte
   */
  [[nodiscard]] auto begin() const -> const uint8_t* { return _span.data(); }

  /**
   * @brief Get const iterator to the end of the pixel data
   * @return Const iterator past the last byte
   */
  [[nodiscard]] auto end() const -> const uint8_t* { return _span.data() + _span.size(); }

  /**
   * @brief Check if the pixel access is empty/invalid
   * @return True if the span is empty, false otherwise
   */
  [[nodiscard]] auto empty() const -> bool { return _span.empty(); }

 private:
  /**
   * @brief Internal constructor used by Surface
   * @param span Span view of the pixel data
   */
  explicit ConstPixelAccess(std::span<const uint8_t> span) : _span(span) {}

  std::span<const uint8_t> _span;
};

}  // namespace sdl

#endif  // SDL_PIXEL_ACCESS_H
