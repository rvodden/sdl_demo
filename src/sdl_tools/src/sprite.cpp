#include <memory>
#include <utility>

#include "sdl/rectangle.h"
#include "sdl/sprite.h"

#include "sprite_impl.h"

namespace sdl::tools {

Sprite::Sprite(std::shared_ptr<const Texture> texture,
               const sdl::Rectangle<float>& rectangle)
    : _impl{std::make_unique<SpriteImpl>(std::move(texture), rectangle)} {
}

Sprite::Sprite(Sprite&& other) noexcept
    : _impl{std::move(other._impl)} {}

auto Sprite::operator=(Sprite&& other) noexcept -> Sprite& {
  std::swap(_impl, other._impl);
  return *this;
}

Sprite::~Sprite() = default;

auto Sprite::getImpl() const -> const SpriteImpl& {
  return *_impl;
}

}  // namespace sdl::tools
