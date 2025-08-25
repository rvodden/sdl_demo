#include "rectangle.h"
#include "include/rectangle_impl.h"

namespace sdlpp {

// Template explicit instantiations
template class Rectangle<int32_t>;
template class Rectangle<float>;

// Constructor
template<typename T>
Rectangle<T>::Rectangle(T x, T y, T width, T height)
    : impl_(std::make_unique<RectangleImpl<T>>(x, y, width, height)) {
}

// Copy constructor
template<typename T>
Rectangle<T>::Rectangle(const Rectangle& other)
    : impl_(std::make_unique<RectangleImpl<T>>(*other.impl_)) {
}

// Move constructor
template<typename T>
Rectangle<T>::Rectangle(Rectangle&& other) noexcept = default;

// Destructor
template<typename T>
Rectangle<T>::~Rectangle() = default;

// Copy assignment operator
template<typename T>
auto Rectangle<T>::operator=(const Rectangle& other) -> Rectangle& {
    if (this != &other) {
        impl_ = std::make_unique<RectangleImpl<T>>(*other.impl_);
    }
    return *this;
}

// Move assignment operator
template<typename T>
auto Rectangle<T>::operator=(Rectangle&& other) noexcept -> Rectangle& = default;

// Getters
template<typename T>
auto Rectangle<T>::getX() const -> T {
    return impl_->getSDLRect()->x;
}

template<typename T>
auto Rectangle<T>::getY() const -> T {
    return impl_->getSDLRect()->y;
}

template<typename T>
auto Rectangle<T>::getWidth() const -> T {
    return impl_->getSDLRect()->w;
}

template<typename T>
auto Rectangle<T>::getHeight() const -> T {
    return impl_->getSDLRect()->h;
}

// Setters
template<typename T>
auto Rectangle<T>::setX(T x) -> void {
    impl_->getSDLRect()->x = x;
}

template<typename T>
auto Rectangle<T>::setY(T y) -> void {
    impl_->getSDLRect()->y = y;
}

template<typename T>
auto Rectangle<T>::setWidth(T width) -> void {
    impl_->getSDLRect()->w = width;
}

template<typename T>
auto Rectangle<T>::setHeight(T height) -> void {
    impl_->getSDLRect()->h = height;
}

// Contains method
template<typename T>
auto Rectangle<T>::contains(T x, T y) const -> bool {
    const auto* rect = impl_->getSDLRect();
    return (x >= rect->x && y >= rect->y && 
            x < rect->x + rect->w && y < rect->y + rect->h);
}

}  // namespace sdlpp