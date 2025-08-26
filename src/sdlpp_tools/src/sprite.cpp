#include "sprite_impl.h"

namespace sdlpp::tools {

Sprite::Sprite(std::shared_ptr<const Texture> texture,
               const sdlpp::Rectangle<float>& rectangle)
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

}  // namespace sdlpp::tools
