#ifndef SDL_WINDOW_H
#define SDL_WINDOW_H

#include <memory>
#include <string>

#include "sdl_export.h"

namespace sdl {

class Renderer;
class WindowImpl;

/**!
 * @brief A window with
 */
class SDL_EXPORT Window {
  friend Renderer;

 public:
  Window(std::string title, uint16_t width, uint16_t height, uint32_t flags);
  Window(Window&& other) noexcept;
  Window(const Window& other) = delete;
  ~Window();

  auto operator=(const Window& other) -> Window& = delete;
  auto operator=(Window&& other) noexcept -> Window&;

  //! @brief Get the title of the window.
  auto getTitle() -> std::string_view;

  //! @brief Set the title of the window.
  void setTitle(const std::string& newTitle);

 private:
  std::string _title;
  std::unique_ptr<WindowImpl> _windowImpl;
};

}  // namespace sdl

#endif
