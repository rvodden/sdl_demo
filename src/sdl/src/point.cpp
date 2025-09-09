#include <sdl/point.h>

#include "point_impl.h"

namespace sdl {
  
template class Point<int32_t>;
template class Point<float>;

template<PointCoordinate T>
Point<T>::Point(T x, T y) : _impl(std::make_unique<PointImpl<T>>(x, y)) {}

template<PointCoordinate T>
Point<T>::Point(const Point& other) : _impl(std::make_unique<PointImpl<T>>(other.getX(), other.getY())) {}

template<PointCoordinate T>
Point<T>::Point(Point&& other) noexcept = default;

template<PointCoordinate T>
Point<T>::~Point() = default;

template<PointCoordinate T>
auto Point<T>::operator=(const Point& other) -> Point& {
    if (this != &other) {
        _impl = std::make_unique<PointImpl<T>>(other.getX(), other.getY());
    }
    return *this;
}

template<PointCoordinate T>
auto Point<T>::operator=(Point&& other) noexcept -> Point& = default;

template<PointCoordinate T>
auto Point<T>::getX() const -> T { return _impl->getSDLPoint()->x; }

template<PointCoordinate T>
auto Point<T>::getY() const -> T { return _impl->getSDLPoint()->y; }


}
