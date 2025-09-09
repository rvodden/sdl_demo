#ifndef POINT_IMPL_H
#define POINT_IMPL_H

#include <SDL3/SDL.h>

#include <sdl/point.h>

namespace sdl {

template<PointCoordinate T>
class PointImpl {};

template<>
class PointImpl<int32_t> {
 public:
  PointImpl() : _point{0,0} {}
  PointImpl(SDL_Point sdlPoint) : _point{sdlPoint} {}
  PointImpl(int32_t x, int32_t y) : _point { x , y } {}

  auto getSDLPoint() -> SDL_Point* { return &_point; }
  auto getSDLPoint() const -> const SDL_Point* { return &_point; }

 private:
  SDL_Point _point;
};

template<>
class PointImpl<float> {
 public:
  PointImpl() : _point{0,0} {}
  PointImpl(SDL_FPoint sdlPoint) : _point{sdlPoint} {}
  PointImpl(float x, float y) : _point { x , y } {}

  auto getSDLPoint() -> SDL_FPoint* { return &_point; }
  auto getSDLPoint() const -> const SDL_FPoint* { return &_point; }
 
 private:
  SDL_FPoint _point;
};

}

#endif // POINT_IMPL_H
