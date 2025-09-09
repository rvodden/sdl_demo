#ifndef SDL_POINT_H
#define SDL_POINT_H

#include <concepts>
#include <cstdint>
#include <memory>
#include <type_traits>

#include "sdl_export.h"

namespace sdl {

/**
 * @brief Concept to restrict Point template to supported coordinate types.
 * 
 * Only int32_t and float are supported as they map directly to SDL3's
 * SDL_Point and SDL_FPoint types respectively.
 */
template<typename T>
concept PointCoordinate = std::same_as<T, int32_t> || std::same_as<T, float>;

// Forward declaration - full definition in point_impl.h
template<PointCoordinate T> class PointImpl;

/**
 * @brief Template-based point class supporting both integer and floating-point coordinates.
 * 
 * This class provides a unified interface for SDL3's SDL_Point (integer) and SDL_FPoint (floating-point)
 * point types. The template parameter T determines the coordinate type and internally maps to the
 * appropriate SDL type.
 * 
 * @tparam T Coordinate type - must be int32_t for integer points or float for floating-point points
 * 
 * @par Supported Types:
 * - Point<int32_t> - Maps to SDL_Point for pixel-perfect integer positioning
 * - Point<float> - Maps to SDL_FPoint for sub-pixel precision
 * 
 * @par Usage Examples:
 * @code
 * // Integer point for pixel-perfect positioning
 * Point<int32_t> intPoint(100, 50);
 * 
 * // Floating-point point for smooth positioning
 * Point<float> floatPoint(100.5f, 50.25f);
 * 
 * // Query coordinates
 * auto x = point.getX();
 * auto y = point.getY();
 * 
 * // Type conversion
 * Point<int32_t> intPoint2(10, 20);
 * Point<float> floatPoint2(intPoint2);  // Explicit conversion
 * @endcode
 */
template<PointCoordinate T>
class SDL_EXPORT Point {
  public:
    /**
     * @brief Constructs a point with specified coordinates.
     * 
     * @param x X-coordinate of the point
     * @param y Y-coordinate of the point
     */
    Point(T x, T y);
    
    /**
     * @brief Copy constructor.
     * 
     * Creates a new point with the same coordinates as the source.
     * 
     * @param other The point to copy from
     */
    Point(const Point& other);
    
    /**
     * @brief Move constructor.
     * 
     * Transfers ownership of the point implementation from the source.
     * 
     * @param other The point to move from (will be left in a valid but unspecified state)
     */
    Point(Point&& other) noexcept;
    
    /**
     * @brief Explicit conversion constructor.
     * 
     * Creates a point by converting from a different coordinate type.
     * Requires explicit conversion between Point<int32_t> and Point<float>.
     * 
     * @tparam U Source coordinate type (must be a valid PointCoordinate)
     * @param other The point to convert from
     * 
     * @par Usage Examples:
     * @code
     * Point<int32_t> intPoint(10, 20);
     * Point<float> floatPoint(intPoint);  // Explicit conversion
     * 
     * Point<float> floatPoint2(10.5f, 20.25f);
     * Point<int32_t> intPoint2(floatPoint2);  // Explicit conversion (truncates)
     * @endcode
     */
    template<PointCoordinate U>
    requires (!std::same_as<T, U>)
    explicit Point(const Point<U>& other) 
        : Point(static_cast<T>(other.getX()), 
               static_cast<T>(other.getY())) {
    }
    
    /**
     * @brief Destructor.
     */
    ~Point();

    /**
     * @brief Copy assignment operator.
     * 
     * Replaces this point's coordinates with those from the source.
     * 
     * @param other The point to copy from
     * @return Reference to this point after assignment
     */
    auto operator=(const Point& other) -> Point&;
    
    /**
     * @brief Move assignment operator.
     * 
     * Transfers ownership of the point implementation from the source.
     * 
     * @param other The point to move from (will be left in a valid but unspecified state)
     * @return Reference to this point after assignment
     */
    auto operator=(Point&& other) noexcept -> Point&;

    /**
     * @brief Gets the X-coordinate of the point.
     * 
     * @return X-coordinate value
     */
    [[nodiscard]] auto getX() const -> T;
    
    /**
     * @brief Gets the Y-coordinate of the point.
     * 
     * @return Y-coordinate value
     */
    [[nodiscard]] auto getY() const -> T;

  private:
    std::unique_ptr<PointImpl<T>> _impl;
};

// Common type aliases for convenience
using IntPoint = Point<int32_t>;
using FloatPoint = Point<float>;

}  // namespace sdl

#endif  // SDL_POINT_H
