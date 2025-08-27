#ifndef SDL_TOOLS_SPRITE_RENDERER_H
#define SDL_TOOLS_SPRITE_RENDERER_H

#include <memory>

#include "renderer.h"
#include "sdl_tools_export.h"
#include "sprite.h"

namespace sdl::tools {

class Sprite;
class SpriteRendererImpl;

class SDL_TOOLS_EXPORT SpriteRenderer {
 public:
  SpriteRenderer(std::shared_ptr<Renderer> renderer);
  SpriteRenderer(const SpriteRenderer&) = delete;
  SpriteRenderer(SpriteRenderer&& other) noexcept;

  auto operator=(const SpriteRenderer& other) -> SpriteRenderer& = delete;
  auto operator=(SpriteRenderer&& other) noexcept -> SpriteRenderer&;

  ~SpriteRenderer();

  //! @brief render the provided sprite at the provided locations
  void render(const Sprite& sprite, float x, float y) const;

 private:
  std::unique_ptr<SpriteRendererImpl> _impl;
};

}  // namespace sdl::tools

#endif
