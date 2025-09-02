#ifndef SDL_RECTANGLE_H
#define SDL_RECTANGLE_H

#include <cstdint>
#include <memory>
#include <type_traits>
#include <concepts>

#include "sdl_export.h"

namespace sdl {

/**
 * @brief Concept to restrict Rectangle template to supported coordinate types.
 * 
 * Only int32_t and float are supported as they map directly to SDL3's
 * SDL_Rect and SDL_FRect types respectively.
 */
template<typename T>
concept RectangleCoordinate = std::same_as<T, int32_t> || std::same_as<T, float>;

class Renderer;

// Forward declaration - full definition in rectangle_impl.h
template<RectangleCoordinate T> class RectangleImpl;

/**
 * @brief Template-based rectangle class supporting both integer and floating-point coordinates.
 * 
 * This class provides a unified interface for SDL3's SDL_Rect (integer) and SDL_FRect (floating-point)
 * rectangle types. The template parameter T determines the coordinate type and internally maps to the
 * appropriate SDL type.
 * 
 * @tparam T Coordinate type - must be int32_t for integer rectangles or float for floating-point rectangles
 * 
 * @par Supported Types:
 * - Rectangle<int32_t> - Maps to SDL_Rect for pixel-perfect integer positioning
 * - Rectangle<float> - Maps to SDL_FRect for sub-pixel precision
 * 
 * @par Usage Examples:
 * @code
 * // Integer rectangle for pixel-perfect positioning
 * Rectangle<int32_t> intRect(10, 20, 100, 50);
 * 
 * // Floating-point rectangle for smooth animations
 * Rectangle<float> floatRect(10.5f, 20.25f, 100.0f, 50.0f);
 * 
 * // Query properties
 * auto x = rect.getX();
 * auto width = rect.getWidth();
 * 
 * // Test point containment
 * bool contains = rect.contains(x, y);
 * @endcode
 */
template<RectangleCoordinate T>
class SDL_EXPORT Rectangle {
    
    friend Renderer;

public:
    /**
     * @brief Constructs a rectangle with specified position and dimensions.
     * 
     * @param x X-coordinate of the top-left corner
     * @param y Y-coordinate of the top-left corner  
     * @param width Width of the rectangle (must be non-negative)
     * @param height Height of the rectangle (must be non-negative)
     */
    Rectangle(T x, T y, T width, T height);

    /**
     * @brief Copy constructor.
     * 
     * Creates a new rectangle with the same position and dimensions as the source.
     * 
     * @param other The rectangle to copy from
     */
    Rectangle(const Rectangle& other);

    /**
     * @brief Move constructor.
     * 
     * Transfers ownership of the rectangle implementation from the source.
     * 
     * @param other The rectangle to move from (will be left in a valid but unspecified state)
     */
    Rectangle(Rectangle&& other) noexcept;

    /**
     * @brief Explicit conversion constructor.
     * 
     * Creates a rectangle by converting from a different coordinate type.
     * Requires explicit conversion between Rectangle<int32_t> and Rectangle<float>.
     * 
     * @tparam U Source coordinate type (must be a valid RectangleCoordinate)
     * @param other The rectangle to convert from
     * 
     * @par Usage Examples:
     * @code
     * Rectangle<int32_t> intRect(10, 20, 100, 50);
     * Rectangle<float> floatRect(intRect);  // Explicit conversion
     * 
     * Rectangle<float> floatRect2(10.5f, 20.25f, 100.0f, 50.0f);
     * Rectangle<int32_t> intRect2(floatRect2);  // Explicit conversion (truncates)
     * @endcode
     */
    template<RectangleCoordinate U>
    requires (!std::same_as<T, U>)
    explicit Rectangle(const Rectangle<U>& other) 
        : Rectangle(static_cast<T>(other.getX()), 
                   static_cast<T>(other.getY()), 
                   static_cast<T>(other.getWidth()), 
                   static_cast<T>(other.getHeight())) {
    }

    /**
     * @brief Destructor.
     */
    ~Rectangle();

    /**
     * @brief Copy assignment operator.
     * 
     * Replaces this rectangle's position and dimensions with those from the source.
     * 
     * @param other The rectangle to copy from
     * @return Reference to this rectangle after assignment
     */
    auto operator=(const Rectangle& other) -> Rectangle&;

    /**
     * @brief Move assignment operator.
     * 
     * Transfers ownership of the rectangle implementation from the source.
     * 
     * @param other The rectangle to move from (will be left in a valid but unspecified state)
     * @return Reference to this rectangle after assignment
     */
    auto operator=(Rectangle&& other) noexcept -> Rectangle&;

    /**
     * @brief Gets the X-coordinate of the rectangle's top-left corner.
     * 
     * @return X-coordinate value
     */
    [[nodiscard]] auto getX() const -> T;

    /**
     * @brief Gets the Y-coordinate of the rectangle's top-left corner.
     * 
     * @return Y-coordinate value
     */
    [[nodiscard]] auto getY() const -> T;

    /**
     * @brief Gets the width of the rectangle.
     * 
     * @return Width value (always non-negative)
     */
    [[nodiscard]] auto getWidth() const -> T;

    /**
     * @brief Gets the height of the rectangle.
     * 
     * @return Height value (always non-negative)
     */
    [[nodiscard]] auto getHeight() const -> T;

    /**
     * @brief Tests whether a point is contained within the rectangle.
     * 
     * A point is considered contained if it lies within the rectangle's bounds,
     * including the top and left edges but excluding the bottom and right edges.
     * This follows SDL's standard rectangle containment semantics.
     * 
     * @param x X-coordinate of the point to test
     * @param y Y-coordinate of the point to test
     * @return true if the point is contained within the rectangle, false otherwise
     * 
     * @note For integer rectangles, containment is: x >= rect.x && y >= rect.y && 
     *       x < rect.x + rect.width && y < rect.y + rect.height
     * @note For floating-point rectangles, the same logic applies with floating-point arithmetic
     */
    [[nodiscard]] auto contains(T x, T y) const -> bool;

    /**
     * @brief Tests whether this rectangle intersects with another rectangle.
     * 
     * Two rectangles intersect if they share any area in common.
     * 
     * @param other The other rectangle to test intersection with
     * @return true if the rectangles intersect, false otherwise
     */
    [[nodiscard]] auto hasIntersection(const Rectangle<T>& other) const -> bool;

    /**
     * @brief Computes the intersection of this rectangle with another rectangle.
     * 
     * If the rectangles intersect, returns a new rectangle representing the overlapping area.
     * If they do not intersect, returns a rectangle with zero width and height at (0,0).
     * 
     * @param other The other rectangle to compute intersection with
     * @return A new rectangle representing the intersection area, or a zero-area rectangle if no intersection
     */
    [[nodiscard]] auto getIntersection(const Rectangle<T>& other) const -> Rectangle<T>;

    /**
     * @brief Sets the X-coordinate of the rectangle's top-left corner.
     * 
     * @param x New X-coordinate value
     */
    auto setX(T x) -> void;

    /**
     * @brief Sets the Y-coordinate of the rectangle's top-left corner.
     * 
     * @param y New Y-coordinate value
     */
    auto setY(T y) -> void;

    /**
     * @brief Sets the width of the rectangle.
     * 
     * @param width New width value (should be non-negative)
     */
    auto setWidth(T width) -> void;

    /**
     * @brief Sets the height of the rectangle.
     * 
     * @param height New height value (should be non-negative)
     */
    auto setHeight(T height) -> void;

private:
    std::unique_ptr<RectangleImpl<T>> impl_;
};

// Common type aliases for convenience
using IntRectangle = Rectangle<int32_t>;
using FloatRectangle = Rectangle<float>;

}  // namespace sdl

#endif  // SDL_RECTANGLE_H
