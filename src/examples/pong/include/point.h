#ifndef POINT_H
#define POINT_H

#include <concepts>

template<class T> concept Numeric = std::is_arithmetic_v<T>;

namespace pong {

template<Numeric T>
class Point {
 public:
  constexpr Point(T x_ = T{}, T y_ = T{}) : x(x_), y(y_) {};

  auto operator+(const Point& rhs) const -> Point {
    return Point(x + rhs.x, y + rhs.y);
  }

  auto operator+=(const Point& rhs) -> Point& {
    x += rhs.x;
    y += rhs.y;      
    return *this;
  }

  auto operator*(T scalar) const -> Point {
    return Point(x * scalar, y * scalar);
  }

  auto operator-() const -> Point {
    return {-x, -y};
  };

  T x;
  T y;
};

} // namespace pong

#endif  // POINT_H
