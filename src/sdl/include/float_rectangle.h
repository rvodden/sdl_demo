#ifndef SDL_RECTANGLE_H
#define SDL_RECTANGLE_H

#include <memory>

#include "renderer.h"

namespace sdl {

class Renderer;
class FloatRectangleImpl;

class FloatRectangle {
  friend Renderer;
  public:
    FloatRectangle(float topLeftX, float topLeftY, float width, float height);

    /**
     * FloatRectangleImpl is incomplete here, so the compiler cannot
     * construct default move & copy constructors & assignemnt
     * operators. In rectangle.cpp FloatRectangleImpl is complete so
     * we just have to type out these methods manually
    */

    //! @brief copy constructor
    FloatRectangle(const FloatRectangle& other);
    //! @brief move constructor
    FloatRectangle(FloatRectangle&& other) noexcept;

    ~FloatRectangle();

    //! @brief copy assignment operator
    auto operator=(const FloatRectangle& other) -> FloatRectangle&;
    
    //! @brief move assignment operator
    auto operator=(FloatRectangle&& other) noexcept -> FloatRectangle&;

    [[nodiscard]] auto getX() const -> float;
    [[nodiscard]] auto getY() const -> float;
    [[nodiscard]] auto getHeight() const -> float;
    [[nodiscard]] auto getWidth() const -> float;

    [[nodiscard]] auto contains(const float &topLeftX, const float &topLeftY) const -> bool;

  private:
    std::unique_ptr<FloatRectangleImpl> _rectangleImpl;
};

}

#endif
