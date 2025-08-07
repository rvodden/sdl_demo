#include <iostream>

#include "sprite_renderer_impl.h"

#include "sprite.h"
#include "sprite_impl.h"
#include "sprite_renderer.h"

namespace sdl::tools {

SpriteRenderer::SpriteRenderer(const Renderer& renderer) : _spriteRendererImpl { std::make_unique<SpriteRendererImpl>(renderer) } {}
SpriteRenderer::SpriteRenderer(SpriteRenderer &&other) : _spriteRendererImpl { std::move(other._spriteRendererImpl ) } {}

SpriteRenderer::~SpriteRenderer() {};

void SpriteRenderer::render(const Sprite &sprite, const float x, const float y) const
{
  const FloatRectangle destination = {
      x,
      y,
      sprite._spriteImpl->_rectangle.getWidth(),
      sprite._spriteImpl->_rectangle.getHeight()};

  _spriteRendererImpl->_renderer.copy(sprite._spriteImpl->_spriteSheet, sprite._spriteImpl->_rectangle, destination);
}

}
