#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include <string>
#include <memory>

namespace sdl {

class Renderer;
class WindowImpl;

/**!
 * @brief A window with
 */
class Window {
  friend Renderer;
  public:
    Window(std::string title, uint16_t width, uint16_t height, uint32_t flags);
    Window(Window&& other) noexcept;
    Window(const Window& other) = delete;
    ~Window();

    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) noexcept;
    
    //! @brief Get the title of the window.
    std::string_view getTitle();

    //! @brief Set the title of the window.
    void setTitle(std::string newTitle);

  private:
    std::string _title;
    std::unique_ptr<WindowImpl> _windowImpl;
};

}

#endif
