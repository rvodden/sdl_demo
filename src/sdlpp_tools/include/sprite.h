#ifndef SDL_TOOLS_SPRITE_SHEET_H
#define SDL_TOOLS_SPRITE_SHEET_H

#include <memory>

#include "sdlpp_tools_export.h"
#include "sprite_renderer.h"
#include "texture.h"

namespace sdlpp::tools {

class SpriteImpl;
class SpriteRenderer;

/**
 * @brief A renderable sprite representing a rectangular region of a texture
 *
 * Sprite represents a drawable 2D image or animation frame that references
 * a specific rectangular region within a larger Texture. This design enables
 * efficient sprite sheet usage where multiple sprites can share a single
 * texture atlas, reducing texture memory consumption and improving rendering
 * performance through batching.
 *
 * Sprites maintain a reference to their source texture and define their
 * region using a FloatRectangle, allowing for pixel-perfect extraction
 * from sprite sheets, tile sets, and texture atlases. The sprite itself
 * is lightweight and can be efficiently copied and managed.
 *
 * Sprites are rendered through the SpriteRenderer system, which handles
 * batching, transform calculations, and efficient GPU submission. Multiple
 * sprites can be rendered together in a single draw call when they share
 * the same texture, dramatically improving performance for complex scenes.
 *
 * The sprite system supports fractional coordinates for smooth animation
 * and sub-pixel positioning, making it ideal for 2D games, UI elements,
 * and animated content that requires precise control over visual placement.
 *
 * @note Sprites cannot be copied, only moved for performance reasons
 * @note The source texture must remain valid for the lifetime of all sprites using it
 * @note Sprites are lightweight - the texture data is shared, not duplicated
 * @see SpriteRenderer for efficient batch rendering of multiple sprites
 * @see Texture for the underlying graphics resource
 * @see FloatRectangle for the sprite region definition
 *
 * Example usage:
 * @code
 * // Load sprite sheet texture
 * sdlpp::Texture spriteSheet(renderer, "characters.png");
 * auto sharedTexture = std::make_shared<const sdlpp::Texture>(std::move(spriteSheet));
 * 
 * // Create sprites from different regions of the same texture
 * sdlpp::tools::Sprite playerIdle(sharedTexture, FloatRectangle(0, 0, 32, 48));
 * sdlpp::tools::Sprite playerWalk1(sharedTexture, FloatRectangle(32, 0, 32, 48));
 * sdlpp::tools::Sprite playerWalk2(sharedTexture, FloatRectangle(64, 0, 32, 48));
 * 
 * // Render multiple sprites efficiently
 * sdlpp::tools::SpriteRenderer spriteRenderer(renderer);
 * spriteRenderer.addSprite(playerIdle, 100, 200);   // Position player
 * spriteRenderer.addSprite(playerWalk1, 200, 200);  // Position NPC
 * spriteRenderer.render(); // Batch render all sprites
 * @endcode
 */
class SDLPP_TOOLS_EXPORT Sprite {
  friend SpriteRenderer;

 public:
  /**
   * @brief Create a sprite from a texture region
   * @param texture Shared pointer to the source texture (must remain valid)
   * @param rectangle The rectangular region within the texture to use for this sprite
   * @throws sdlpp::Exception if sprite creation fails or texture is invalid
   *
   * Creates a new sprite that references a specific rectangular region within
   * the provided texture. The rectangle coordinates are in texture space,
   * where (0,0) represents the top-left corner of the texture.
   *
   * The texture is stored as a shared pointer, allowing multiple sprites to
   * safely reference the same texture data. This is essential for sprite sheet
   * workflows where many sprites are extracted from a single large texture.
   *
   * The rectangle defines which portion of the texture this sprite represents.
   * It can be any valid region within the texture bounds, allowing for flexible
   * sprite extraction from tile sets, character sheets, or UI element atlases.
   *
   * @note The texture must remain valid for the lifetime of this sprite
   * @note Rectangle coordinates are in pixels relative to texture top-left (0,0)
   * @note Fractional coordinates are supported for sub-pixel sprite regions
   * @note Multiple sprites can safely share the same texture
   *
   * Example sprite sheet extraction:
   * @code
   * // 256x256 texture with 32x32 tiles in an 8x8 grid
   * auto texture = std::make_shared<const Texture>(renderer, "tileset.png");
   * 
   * // Extract tile at row 2, column 5 (0-indexed)
   * FloatRectangle tileRegion(5 * 32, 2 * 32, 32, 32);
   * Sprite grassTile(texture, tileRegion);
   * 
   * // Extract animated frames from a character sheet
   * std::vector<Sprite> walkAnimation;
   * for (int frame = 0; frame < 4; ++frame) {
   *     FloatRectangle frameRegion(frame * 64, 0, 64, 64);
   *     walkAnimation.emplace_back(texture, frameRegion);
   * }
   * @endcode
   */
  Sprite(std::shared_ptr<const Texture> texture,
         const FloatRectangle& rectangle);
         
  /**
   * @brief Copy constructor is explicitly deleted
   * @note Sprites cannot be copied due to performance considerations with texture references
   */
  Sprite(const Sprite& other) = delete;
  
  /**
   * @brief Move constructor - transfers ownership of the sprite data
   * @param other The sprite to move from (will be left in valid but unspecified state)
   * @note The moved-from sprite should not be used except for destruction
   * @note Texture references are transferred to maintain validity
   */
  Sprite(Sprite&& other) noexcept;

  /**
   * @brief Copy assignment is explicitly deleted
   * @note Sprites cannot be copied due to performance considerations with texture references
   */
  auto operator=(const Sprite& other) -> Sprite& = delete;
  
  /**
   * @brief Move assignment operator - transfers ownership of the sprite data
   * @param other The sprite to move from (will be left in valid but unspecified state)
   * @return Reference to this sprite
   * @note The moved-from sprite should not be used except for destruction
   * @note Texture references are transferred to maintain validity
   */
  auto operator=(Sprite&& other) noexcept -> Sprite&;

  /**
   * @brief Destructor - automatically releases sprite resources
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   * @note The shared texture reference is automatically decremented
   * @note Does not affect other sprites sharing the same texture
   */
  ~Sprite();

 private:
  std::unique_ptr<SpriteImpl> _spriteImpl;
};

}  // namespace sdlpp::tools

#endif
