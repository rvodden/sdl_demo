#include "rectangle_impl.h"
#include "rectangle.h"

namespace sdl {

Rectangle::Rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height) : _rectangleImpl {std::make_unique<RectangleImpl>() } {
  _rectangleImpl->_sdlRect.x = x;
  _rectangleImpl->_sdlRect.y = y;
  _rectangleImpl->_sdlRect.w = width;
  _rectangleImpl->_sdlRect.h = height;
}

Rectangle::Rectangle(Rectangle &other) : _rectangleImpl{ std::make_unique<RectangleImpl>(other._rectangleImpl->_sdlRect) } { }

Rectangle::Rectangle(Rectangle &&other) : _rectangleImpl{ std::move(other._rectangleImpl) } {}

Rectangle::~Rectangle(){};

Rectangle &Rectangle::operator=(Rectangle &other) {
  _rectangleImpl = std::make_unique<RectangleImpl>(other._rectangleImpl->_sdlRect);
  return *this;
}

Rectangle &Rectangle::operator=(Rectangle &&other){
  _rectangleImpl = std::move(other._rectangleImpl);
  return *this;
}

uint32_t Rectangle::getX()
{
  return _rectangleImpl->_sdlRect.x;
}

uint32_t Rectangle::getY()
{
  return _rectangleImpl->_sdlRect.y;
}

uint32_t Rectangle::getHeight()
{
  return _rectangleImpl->_sdlRect.h;
}

uint32_t Rectangle::getWidth()
{
  return _rectangleImpl->_sdlRect.w;
};

bool Rectangle::contains(const uint32_t &x, const uint32_t &y) {
  uint32_t rx = getX();
  uint32_t ry = getY();
  uint32_t height = getHeight();
  uint32_t width = getWidth();

  if ( x < rx ) return false;
  if ( x > rx + width ) return false;
  if ( y < ry ) return false;
  if ( y > ry + height ) return false;

  return true;
}

}
