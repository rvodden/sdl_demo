#include "sdl/rectangle.h"

#include "rectangle_impl.h"

namespace sdl {

// Template explicit instantiations
template class Rectangle<int32_t>;
template class Rectangle<float>;

// Explicit instantiation of conversion constructors
template Rectangle<int32_t>::Rectangle(const Rectangle<float>&);
template Rectangle<float>::Rectangle(const Rectangle<int32_t>&);

// Constructor
template<RectangleCoordinate T>
Rectangle<T>::Rectangle(T x, T y, T width, T height)
    : impl_(std::make_unique<RectangleImpl<T>>(x, y, width, height)) {
}

// Copy constructor
template<RectangleCoordinate T>
Rectangle<T>::Rectangle(const Rectangle& other)
    : impl_(std::make_unique<RectangleImpl<T>>(*other.impl_)) {
}

// Move constructor
template<RectangleCoordinate T>
Rectangle<T>::Rectangle(Rectangle&& other) noexcept = default;

// Conversion constructor
template<RectangleCoordinate T>
template<RectangleCoordinate U>
requires (!std::same_as<T, U>)
Rectangle<T>::Rectangle(const Rectangle<U>& other)
    : impl_(std::make_unique<RectangleImpl<T>>(
        static_cast<T>(other.getX()),
        static_cast<T>(other.getY()),
        static_cast<T>(other.getWidth()),
        static_cast<T>(other.getHeight()))) {
}

// Destructor
template<RectangleCoordinate T>
Rectangle<T>::~Rectangle() = default;

// Copy assignment operator
template<RectangleCoordinate T>
auto Rectangle<T>::operator=(const Rectangle& other) -> Rectangle& {
    if (this != &other) {
        impl_ = std::make_unique<RectangleImpl<T>>(*other.impl_);
    }
    return *this;
}

// Move assignment operator
template<RectangleCoordinate T>
auto Rectangle<T>::operator=(Rectangle&& other) noexcept -> Rectangle& = default;

// Getters
template<RectangleCoordinate T>
auto Rectangle<T>::getX() const -> T {
    return impl_->getSDLRect()->x;
}

template<RectangleCoordinate T>
auto Rectangle<T>::getY() const -> T {
    return impl_->getSDLRect()->y;
}

template<RectangleCoordinate T>
auto Rectangle<T>::getWidth() const -> T {
    return impl_->getSDLRect()->w;
}

template<RectangleCoordinate T>
auto Rectangle<T>::getHeight() const -> T {
    return impl_->getSDLRect()->h;
}

// Setters
template<RectangleCoordinate T>
auto Rectangle<T>::setX(T x) -> void {
    impl_->getSDLRect()->x = x;
}

template<RectangleCoordinate T>
auto Rectangle<T>::setY(T y) -> void {
    impl_->getSDLRect()->y = y;
}

template<RectangleCoordinate T>
auto Rectangle<T>::setWidth(T width) -> void {
    impl_->getSDLRect()->w = width;
}

template<RectangleCoordinate T>
auto Rectangle<T>::setHeight(T height) -> void {
    impl_->getSDLRect()->h = height;
}

template<>
auto Rectangle<float>::contains(float x, float y) const -> bool {
  SDL_FPoint point{ .x = x, .y = y };
  return SDL_PointInRectFloat(&point, impl_->getSDLRect());
}

template<>
auto Rectangle<int32_t>::contains(int32_t x, int32_t y) const -> bool {
  SDL_Point point { .x = x, .y = y };
  return SDL_PointInRect(&point, impl_->getSDLRect());
}

template<>
auto Rectangle<float>::hasIntersection(const Rectangle<float>& other) const -> bool {
  return SDL_HasRectIntersectionFloat(impl_->getSDLRect(), other.impl_->getSDLRect());
}

template<>
auto Rectangle<int32_t>::hasIntersection(const Rectangle<int32_t>& other) const -> bool {
  return SDL_HasRectIntersection(impl_->getSDLRect(), other.impl_->getSDLRect());
}

template<>
auto Rectangle<float>::getIntersection(const Rectangle<float>& other) const -> Rectangle<float> {
  SDL_FRect result;
  SDL_GetRectIntersectionFloat(impl_->getSDLRect(), other.impl_->getSDLRect(), &result);
  return Rectangle<float>(result.x, result.y, result.w, result.h);
}

template<>
auto Rectangle<int32_t>::getIntersection(const Rectangle<int32_t>& other) const -> Rectangle<int32_t> {
  SDL_Rect result;
  SDL_GetRectIntersection(impl_->getSDLRect(), other.impl_->getSDLRect(), &result);
  return Rectangle<int32_t>(result.x, result.y, result.w, result.h);
}

}  // namespace sdl
