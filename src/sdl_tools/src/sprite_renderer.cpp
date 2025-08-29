#include <memory>
#include <utility>

#include "sdl/renderer.h"
#include "sdl/rectangle.h"
#include "sdl/sprite.h"
#include "sdl/sprite_renderer.h"

#include "sprite_impl.h"
#include "sprite_renderer_impl.h"

namespace sdl::tools {

SpriteRenderer::SpriteRenderer(std::shared_ptr<Renderer> renderer)
    : _impl{
          std::make_unique<SpriteRendererImpl>(std::move(renderer))} {}
SpriteRenderer::SpriteRenderer(SpriteRenderer&& other) noexcept
    : _impl{std::move(other._impl)} {}

auto SpriteRenderer::operator=(SpriteRenderer&& other) noexcept
    -> SpriteRenderer& {
  std::swap(_impl, other._impl);
  return *this;
}

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::render(const Sprite& sprite, const float x,
                            const float y) const {
  const auto& spriteImpl = sprite.getImpl();
  const sdl::Rectangle<float> destination(
      x, y, spriteImpl.getRectangle().getWidth(),
      spriteImpl.getRectangle().getHeight());

  _impl->_renderer->copy(*spriteImpl.getSpriteSheet(),
                                       spriteImpl.getRectangle(),
                                       destination);
}

}  // namespace sdl::tools
