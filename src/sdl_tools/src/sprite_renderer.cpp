#include "sprite_renderer_impl.h"

#include "sprite.h"
#include "sprite_impl.h"

namespace sdl::tools {

SpriteRenderer::SpriteRenderer(const Renderer& renderer) : _spriteRendereImpl { std::make_unique<SpriteRendererImpl>(renderer) } {};

SpriteRenderer::~SpriteRenderer() {};

void SpriteRenderer::render(const Sprite& sprite, const uint32_t x, const uint32_t y) const {
  const Rectangle destination = { 
    x, 
    y, 
    x + sprite._spriteImpl->_rectangle.getWidth(),
    y + sprite._spriteImpl->_rectangle.getHeight()
  };

  _spriteRendereImpl->_renderer.copy(sprite._spriteImpl->_texture, sprite._spriteImpl->_rectangle, destination);
}

}
