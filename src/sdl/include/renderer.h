#ifndef __SDL_RENDERER_H__
#define __SDL_RENDERER_H__

#include <optional>
#include <memory>
#include <unordered_set>

#include "color.h"
#include "rectangle.h"
#include "texture.h"

namespace sdl {

class Rectangle;
class Texture;
class Window;
class RendererImpl;

//! @brief A 2D Rendering Context
class Renderer {
  friend Texture;
  public:
    typedef uint8_t RendererFlag;
    
    /**
     * @brief Construct a renderer associated with the provided window
     */
    Renderer( Window& window, int16_t index, const std::unordered_set<RendererFlag> flags );
    //! @brief move constructor
    Renderer( Renderer&& other ) noexcept;
    //! @brief Renderer cannot be copied
    Renderer( const Renderer& other ) = delete;
    //! @brief destructor
    ~Renderer() noexcept;

    //! @brief move assignment operator
    Renderer& operator=( Renderer&& other ) noexcept;
    //! @brief Renderer cannot be copied
    Renderer& operator= ( const Renderer& other ) = delete;

    //! @brief Set the color used for drawing operations (Rect, Line and Clear).
    void setRenderDrawColour(const Color& color);

    //! @brief Copy a region of the texture to a region of the renderer.
    const Renderer &copy(
      const Texture& texture
    ) const;
    
    //! @brief Copy a region of the texture to a region of the renderer.
    const Renderer &copy(
      const Texture& texture, 
      const Rectangle &source, 
      const Rectangle &destination
    ) const;

    //! @brief Clear the renderer with the drawing color.
    void clear();

    //! @brief Update the screen with any rendering performed since the previous call.
    void present();

    static constexpr RendererFlag kSoftware = 0;
    static constexpr RendererFlag kAccelerated = 1;
    static constexpr RendererFlag kPresentVSync = 2;
    static constexpr RendererFlag kTargetTexture = 3;

  private:
    std::unique_ptr<RendererImpl> _rendererImpl;
};

}

#endif
