#ifndef __SDL_RECTANGLE_H__
#define __SDL_RECTANGLE_H__

#include <memory>

#include "renderer.h"

namespace sdl {

class Renderer;
class FloatRectangleImpl;

class FloatRectangle {
  friend Renderer;
  public:
    FloatRectangle(float x, float y, float width, float height);

    /**
     * FloatRectangleImpl is incomplete here, so the compiler cannot
     * construct default move & copy constructors & assignemnt
     * operators. In rectangle.cpp FloatRectangleImpl is complete so
     * we just have to type out these methods manually
    */

    //! @brief copy constructor
    FloatRectangle(const FloatRectangle& other);
    //! @brief move constructor
    FloatRectangle(FloatRectangle&& other);

    ~FloatRectangle();

    //! @brief copy assignment operator
    FloatRectangle& operator=(const FloatRectangle& other);
    
    //! @brief move assignment operator
    FloatRectangle& operator=(FloatRectangle&& other);

    float getX() const;
    float getY() const;
    float getHeight() const;
    float getWidth() const;

    bool contains(const float &x, const float &y) const;

  private:
    std::unique_ptr<FloatRectangleImpl> _rectangleImpl;
};

}

#endif
