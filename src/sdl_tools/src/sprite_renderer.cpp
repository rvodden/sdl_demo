#include <iostream>

#include "sprite_renderer_impl.h"

#include "sprite.h"
#include "sprite_impl.h"
#include "sprite_renderer.h"

namespace sdl::tools {

SpriteRenderer::SpriteRenderer(const Renderer& renderer) : _spriteRendererImpl { std::make_unique<SpriteRendererImpl>(renderer) } {}
SpriteRenderer::SpriteRenderer(SpriteRenderer &&other) : _spriteRendererImpl { std::move(other._spriteRendererImpl ) } {}

SpriteRenderer::~SpriteRenderer() {};

void SpriteRenderer::render(const Sprite &sprite, const uint32_t x, const uint32_t y) const
{
  std::cout << "I am rendering a sprite at (" << x << ", " << y << ");" << std::endl;
  const Rectangle destination = {
      x,
      y,
      sprite._spriteImpl->_rectangle.getWidth(),
      sprite._spriteImpl->_rectangle.getHeight()};

  _spriteRendererImpl->_renderer.copy(sprite._spriteImpl->_texture, sprite._spriteImpl->_rectangle, destination);
  _spriteRendererImpl->_renderer.present(); // TODO: doing this here is horribly inefficient
}
}
