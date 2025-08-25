#ifndef SDLPP_RECTANGLE_IMPL_H
#define SDLPP_RECTANGLE_IMPL_H

#include <SDL3/SDL.h>
#include <cstdint>

#include "rectangle.h"

namespace sdlpp {

/**
 * @brief Implementation base template for Rectangle.
 * 
 * This template is specialized for supported coordinate types to provide
 * direct mapping to the appropriate SDL rectangle type without runtime overhead.
 */
template<typename T>
class RectangleImpl {
    static_assert(std::is_same_v<T, int32_t> || std::is_same_v<T, float>,
                  "RectangleImpl only supports int32_t and float coordinate types");
};

/**
 * @brief Specialization for int32_t coordinates using SDL_Rect.
 * 
 * This specialization maps directly to SDL3's SDL_Rect structure for
 * pixel-perfect integer positioning. All operations are performed with
 * zero overhead compared to using SDL_Rect directly.
 */
template<>
class RectangleImpl<int32_t> {
    friend Rectangle<int32_t>;

public:
    /**
     * @brief Default constructor initializing to zero position and size.
     */
    RectangleImpl() : rect_{0, 0, 0, 0} {}

    /**
     * @brief Constructs from SDL_Rect.
     * 
     * @param sdlRect The SDL_Rect to initialize from
     */
    explicit RectangleImpl(SDL_Rect sdlRect) : rect_{sdlRect} {}

    /**
     * @brief Constructs with specified position and dimensions.
     * 
     * @param x X-coordinate of top-left corner
     * @param y Y-coordinate of top-left corner
     * @param width Rectangle width
     * @param height Rectangle height
     */
    RectangleImpl(int32_t x, int32_t y, int32_t width, int32_t height) 
        : rect_{x, y, width, height} {}

    /**
     * @brief Gets pointer to the underlying SDL_Rect for SDL API calls.
     * 
     * @return Pointer to the SDL_Rect structure
     */
    auto getSDLRect() -> SDL_Rect* { return &rect_; }

    /**
     * @brief Gets const pointer to the underlying SDL_Rect.
     * 
     * @return Const pointer to the SDL_Rect structure
     */
    auto getSDLRect() const -> const SDL_Rect* { return &rect_; }

private:
    SDL_Rect rect_;
};

/**
 * @brief Specialization for float coordinates using SDL_FRect.
 * 
 * This specialization maps directly to SDL3's SDL_FRect structure for
 * sub-pixel precision positioning. All operations are performed with
 * zero overhead compared to using SDL_FRect directly.
 */
template<>
class RectangleImpl<float> {
    friend Rectangle<float>;

public:
    /**
     * @brief Default constructor initializing to zero position and size.
     */
    RectangleImpl() : rect_{0.0f, 0.0f, 0.0f, 0.0f} {}

    /**
     * @brief Constructs from SDL_FRect.
     * 
     * @param sdlRect The SDL_FRect to initialize from
     */
    explicit RectangleImpl(SDL_FRect sdlRect) : rect_{sdlRect} {}

    /**
     * @brief Constructs with specified position and dimensions.
     * 
     * @param x X-coordinate of top-left corner
     * @param y Y-coordinate of top-left corner
     * @param width Rectangle width
     * @param height Rectangle height
     */
    RectangleImpl(float x, float y, float width, float height) 
        : rect_{x, y, width, height} {}

    /**
     * @brief Gets pointer to the underlying SDL_FRect for SDL API calls.
     * 
     * @return Pointer to the SDL_FRect structure
     */
    auto getSDLRect() -> SDL_FRect* { return &rect_; }

    /**
     * @brief Gets const pointer to the underlying SDL_FRect.
     * 
     * @return Const pointer to the SDL_FRect structure
     */
    auto getSDLRect() const -> const SDL_FRect* { return &rect_; }

private:
    SDL_FRect rect_;
};

}  // namespace sdlpp

#endif  // SDLPP_RECTANGLE_IMPL_H