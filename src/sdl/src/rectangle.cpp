#include "float_rectangle_impl.h"
#include "float_rectangle.h"

namespace sdl {

FloatRectangle::FloatRectangle(float x, float y, float width, float height) : _rectangleImpl {std::make_unique<FloatRectangleImpl>() } {
  _rectangleImpl->_sdlFRect.x = x;
  _rectangleImpl->_sdlFRect.y = y;
  _rectangleImpl->_sdlFRect.w = width;
  _rectangleImpl->_sdlFRect.h = height;
}

FloatRectangle::FloatRectangle(const FloatRectangle &other) : _rectangleImpl{ std::make_unique<FloatRectangleImpl>(other._rectangleImpl->_sdlFRect) } { }

FloatRectangle::FloatRectangle(FloatRectangle &&other) : _rectangleImpl{ std::move(other._rectangleImpl) } {}

FloatRectangle::~FloatRectangle(){};

FloatRectangle &FloatRectangle::operator=(const FloatRectangle &other) {
  _rectangleImpl = std::make_unique<FloatRectangleImpl>(other._rectangleImpl->_sdlFRect);
  return *this;
}

FloatRectangle &FloatRectangle::operator=(FloatRectangle &&other){
  _rectangleImpl = std::move(other._rectangleImpl);
  return *this;
}

float FloatRectangle::getX() const
{
  return _rectangleImpl->_sdlFRect.x;
}

float FloatRectangle::getY() const
{
  return _rectangleImpl->_sdlFRect.y;
}

float FloatRectangle::getHeight() const
{
  return _rectangleImpl->_sdlFRect.h;
}

float FloatRectangle::getWidth() const
{
  return _rectangleImpl->_sdlFRect.w;
};

bool FloatRectangle::contains(const float &x, const float &y) const {
  float rx = getX();
  float ry = getY();
  float height = getHeight();
  float width = getWidth();

  if ( x < rx ) return false;
  if ( x > rx + width ) return false;
  if ( y < ry ) return false;
  if ( y > ry + height ) return false;

  return true;
}

}
