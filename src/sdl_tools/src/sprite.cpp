#include "sprite_impl.h"

namespace sdl::tools {

Sprite::Sprite(std::shared_ptr<const Texture> texture,
               const FloatRectangle& rectangle)
    : _spriteImpl{std::make_unique<SpriteImpl>(std::move(texture), rectangle)} {
}

Sprite::Sprite(Sprite&& other) noexcept
    : _spriteImpl{std::move(other._spriteImpl)} {}

auto Sprite::operator=(Sprite&& other) noexcept -> Sprite& {
  std::swap(_spriteImpl, other._spriteImpl);
  return *this;
}

Sprite::~Sprite() = default;

}  // namespace sdl::tools
