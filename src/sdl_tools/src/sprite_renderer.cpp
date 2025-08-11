#include <iostream>

#include "sprite_renderer_impl.h"

#include "sprite.h"
#include "sprite_impl.h"
#include "sprite_renderer.h"

namespace sdl::tools {

SpriteRenderer::SpriteRenderer(std::shared_ptr<Renderer> renderer) : _spriteRendererImpl { std::make_unique<SpriteRendererImpl>(std::move(renderer)) } {}
SpriteRenderer::SpriteRenderer(SpriteRenderer &&other) noexcept : _spriteRendererImpl { std::move(other._spriteRendererImpl ) } {}

auto SpriteRenderer::operator=(SpriteRenderer&& other) noexcept -> SpriteRenderer& {
  std::swap(_spriteRendererImpl, other._spriteRendererImpl);
  return *this;
}

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::render(const Sprite &sprite, const float x, const float y) const
{
  const FloatRectangle destination = {
      x,
      y,
      sprite._spriteImpl->_rectangle.getWidth(),
      sprite._spriteImpl->_rectangle.getHeight()};

  _spriteRendererImpl->_renderer->copy(*sprite._spriteImpl->_spriteSheet, sprite._spriteImpl->_rectangle, destination);
}

}
