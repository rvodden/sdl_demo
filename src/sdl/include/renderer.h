#ifndef __SDL_RENDERER_H__
#define __SDL_RENDERER_H__

#include <memory>

#include "color.h"

namespace sdl {

class Window;
class RendererImpl;

//! @brief A 2D Rendering Context
class Renderer {
  public:
    /**
     * @brief Construct a renderer associated with the provided window
     */
    Renderer( Window& window, int16_t index, uint32_t flags );
    //! @brief move constructor
    Renderer( Renderer&& other );
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

    //! @brief Clear the renderer with the drawing color.
    void clear();

    //! @brief Update the screen with any rendering performed since the previous call.
    void present();

  private:
    std::unique_ptr<RendererImpl> _rendererImpl;
};

}

#endif
