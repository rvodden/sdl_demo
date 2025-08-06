#include "rectangle_impl.h"
#include "rectangle.h"

namespace sdl {

Rectangle::Rectangle(int32_t x, int32_t y, int32_t width, int32_t height) : _rectangleImpl {std::make_unique<RectangleImpl>() } {
  _rectangleImpl->_sdlRect.x = x;
  _rectangleImpl->_sdlRect.y = y;
  _rectangleImpl->_sdlRect.w = width;
  _rectangleImpl->_sdlRect.h = height;
}

Rectangle::Rectangle(const Rectangle &other) : _rectangleImpl{ std::make_unique<RectangleImpl>(other._rectangleImpl->_sdlRect) } { }

Rectangle::Rectangle(Rectangle &&other) : _rectangleImpl{ std::move(other._rectangleImpl) } {}

Rectangle::~Rectangle(){};

Rectangle &Rectangle::operator=(const Rectangle &other) {
  _rectangleImpl = std::make_unique<RectangleImpl>(other._rectangleImpl->_sdlRect);
  return *this;
}

Rectangle &Rectangle::operator=(Rectangle &&other){
  _rectangleImpl = std::move(other._rectangleImpl);
  return *this;
}

int32_t Rectangle::getX() const
{
  return _rectangleImpl->_sdlRect.x;
}

int32_t Rectangle::getY() const
{
  return _rectangleImpl->_sdlRect.y;
}

int32_t Rectangle::getHeight() const
{
  return _rectangleImpl->_sdlRect.h;
}

int32_t Rectangle::getWidth() const
{
  return _rectangleImpl->_sdlRect.w;
};

bool Rectangle::contains(const int32_t &x, const int32_t &y) const {
  int32_t rx = getX();
  int32_t ry = getY();
  int32_t height = getHeight();
  int32_t width = getWidth();

  if ( x < rx ) return false;
  if ( x > rx + width ) return false;
  if ( y < ry ) return false;
  if ( y > ry + height ) return false;

  return true;
}

}
