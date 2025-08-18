#ifndef SDL_TOOLS_SPRITE_SHEET_H
#define SDL_TOOLS_SPRITE_SHEET_H

#include <memory>

#include "sprite_renderer.h"
#include "texture.h"

namespace sdl::tools {

class SpriteImpl;
class SpriteRenderer;

class Sprite {
  friend SpriteRenderer;

 public:
  Sprite(std::shared_ptr<const Texture> texture,
         const FloatRectangle& rectangle);
  Sprite(const Sprite& other) = delete;
  Sprite(Sprite&& other) noexcept;

  auto operator=(const Sprite& other) -> Sprite& = delete;
  auto operator=(Sprite&& other) noexcept -> Sprite&;

  ~Sprite();

 private:
  std::unique_ptr<SpriteImpl> _spriteImpl;
};

}  // namespace sdl::tools

#endif
