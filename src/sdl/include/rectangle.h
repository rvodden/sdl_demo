#ifndef __SDL_RECTANGLE_H__
#define __SDL_RECTANGLE_H__

#include <memory>

#include "renderer.h"

namespace sdl {

class Renderer;
class RectangleImpl;

class Rectangle {
  friend Renderer;
  public:
    Rectangle(int32_t x, int32_t y, int32_t width, int32_t height);

    /**
     * RectangleImpl is incomplete here, so the compiler cannot
     * construct default move & copy constructors & assignemnt
     * operators. In rectangle.cpp RectangleImpl is complete so
     * we just have to type out these methods manually
    */

    //! @brief copy constructor
    Rectangle(const Rectangle& other);
    //! @brief move constructor
    Rectangle(Rectangle&& other);

    ~Rectangle();

    //! @brief copy assignment operator
    Rectangle& operator=(const Rectangle& other);
    
    //! @brief move assignment operator
    Rectangle& operator=(Rectangle&& other);

    int32_t getX() const;
    int32_t getY() const;
    int32_t getHeight() const;
    int32_t getWidth() const;

    bool contains(const int32_t &x, const int32_t &y) const;

  private:
    std::unique_ptr<RectangleImpl> _rectangleImpl;
};

}

#endif
