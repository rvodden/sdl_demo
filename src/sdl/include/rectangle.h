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
    Rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    /**
     * RectangleImpl is incomplete here, so the compiler cannot
     * construct default move & copy constructors & assignemnt
     * operators. In rectangle.cpp RectangleImpl is complete so
     * we just have to type out these methods manually
    */

    //! @brief copy constructor
    Rectangle(Rectangle& other);
    //! @brief move constructor
    Rectangle(Rectangle&& other);

    ~Rectangle();

    //! @brief copy assignment operator
    Rectangle& operator=(Rectangle& other);
    
    //! @brief move assignment operator
    Rectangle& operator=(Rectangle&& other);

    uint32_t getX();
    uint32_t getY();
    uint32_t getHeight();
    uint32_t getWidth();

    bool contains(const uint32_t &x, const uint32_t &y);

  private:
    std::unique_ptr<RectangleImpl> _rectangleImpl;
};

}

#endif
