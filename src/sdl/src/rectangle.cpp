#include "float_rectangle.h"
#include "float_rectangle_impl.h"

namespace sdl {

FloatRectangle::FloatRectangle(float x, float y, float width, float height)
    : _rectangleImpl{std::make_unique<FloatRectangleImpl>()} {
  _rectangleImpl->_sdlFRect.x = x;
  _rectangleImpl->_sdlFRect.y = y;
  _rectangleImpl->_sdlFRect.w = width;
  _rectangleImpl->_sdlFRect.h = height;
}

FloatRectangle::FloatRectangle(const FloatRectangle& other)
    : _rectangleImpl{std::make_unique<FloatRectangleImpl>(
          other._rectangleImpl->_sdlFRect)} {}

FloatRectangle::FloatRectangle(FloatRectangle&& other) noexcept
    : _rectangleImpl{std::move(other._rectangleImpl)} {}

FloatRectangle::~FloatRectangle() = default;

auto FloatRectangle::operator=(const FloatRectangle& other) -> FloatRectangle& {
  if (this == &other) {
    return *this;
  }

  _rectangleImpl =
      std::make_unique<FloatRectangleImpl>(other._rectangleImpl->_sdlFRect);
  return *this;
}

auto FloatRectangle::operator=(FloatRectangle&& other) noexcept
    -> FloatRectangle& {
  _rectangleImpl = std::move(other._rectangleImpl);
  return *this;
}

auto FloatRectangle::getX() const -> float {
  return _rectangleImpl->_sdlFRect.x;
}

auto FloatRectangle::getY() const -> float {
  return _rectangleImpl->_sdlFRect.y;
}

auto FloatRectangle::getHeight() const -> float {
  return _rectangleImpl->_sdlFRect.h;
}

auto FloatRectangle::getWidth() const -> float {
  return _rectangleImpl->_sdlFRect.w;
};

auto FloatRectangle::contains(const float& x, const float& y) const -> bool {
  float rx = getX();
  float ry = getY();
  float height = getHeight();
  float width = getWidth();

  if (x < rx) {
    return false;
  }
  if (x > rx + width) {
    return false;
  }
  if (y < ry) {
    return false;
  }
  if (y > ry + height) {
    return false;
  }

  return true;
}

}  // namespace sdl
