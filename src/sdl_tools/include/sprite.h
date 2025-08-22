#ifndef SDL_TOOLS_SPRITE_SHEET_H
#define SDL_TOOLS_SPRITE_SHEET_H

#include <memory>

#include "sdl_tools_export.h"
#include "sprite_renderer.h"
#include "texture.h"

namespace sdlpp::tools {

class SpriteImpl;
class SpriteRenderer;

class SDL_TOOLS_EXPORT Sprite {
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

}  // namespace sdlpp::tools

#endif
