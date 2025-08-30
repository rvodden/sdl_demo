#ifndef SDL_WINDOW_H
#define SDL_WINDOW_H

#include <memory>
#include <string>

#include "sdl/rectangle.h"

#include "sdl_export.h"

namespace sdl {

class Renderer;
class WindowImpl;

/**
 * @brief A system window for rendering graphics and handling input
 *
 * Window represents a system window that can display graphics content.
 * It serves as the target for rendering operations and the source of
 * input events. Windows are automatically destroyed when the object
 * goes out of scope, ensuring proper resource cleanup.
 *
 * Windows use RAII for automatic resource management - the underlying
 * SDL window is created in the constructor and destroyed in the destructor.
 * This guarantees no resource leaks even in exception scenarios.
 *
 * @note Windows cannot be copied, only moved for performance reasons
 * @note The window becomes visible immediately upon creation
 * @see Renderer for performing rendering operations on this window
 * @see EventBus for receiving input events from this window
 *
 * Example usage:
 * @code
 * // Create a 800x600 window
 * sdl::Window window("My Game", 800, 600, 0);
 * 
 * // Create renderer for drawing
 * sdl::Renderer renderer(window);
 * 
 * // Window title can be changed after creation
 * window.setTitle("My Game - Level 1");
 * @endcode
 */
class SDL_EXPORT Window {

 public:
  /**
   * @brief Create a new window with specified properties
   * @param title The window title displayed in the title bar
   * @param width Window width in pixels (must be > 0)
   * @param height Window height in pixels (must be > 0) 
   * @param flags Window creation flags (0 for default behavior, see SDL documentation for other values)
   * @throws sdl::Exception if window creation fails (e.g., insufficient memory, invalid parameters)
   *
   * Creates a new system window with the specified dimensions and title.
   * The window will be initially visible and positioned automatically by
   * the window manager. The window supports hardware-accelerated rendering
   * when a Renderer is attached.
   *
   * @note The title parameter is copied, so the original string can be safely destroyed
   * @note Window dimensions cannot be changed after creation in the current API
   */
  Window(std::string title, uint16_t width, uint16_t height, uint32_t flags);
  
  /**
   * @brief Move constructor - transfers ownership of the window
   * @param other The window to move from (will be left in valid but unspecified state)
   * @note The moved-from window should not be used except for destruction
   */
  Window(Window&& other) noexcept;
  
  /**
   * @brief Copy constructor is explicitly deleted
   * @note Windows cannot be copied due to the underlying system resource
   */
  Window(const Window& other) = delete;
  
  /**
   * @brief Destructor - automatically destroys the system window
   */
  ~Window();

  /**
   * @brief Copy assignment is explicitly deleted
   * @note Windows cannot be copied due to the underlying system resource
   */
  auto operator=(const Window& other) -> Window& = delete;
  
  /**
   * @brief Move assignment operator - transfers ownership of the window
   * @param other The window to move from (will be left in valid but unspecified state)
   * @return Reference to this window
   * @note The moved-from window should not be used except for destruction
   */
  auto operator=(Window&& other) noexcept -> Window&;

  [[nodiscard]] Rectangle<int32_t> getSize() const;

  /**
   * @brief Get the current title of the window
   * @return A string view of the window title (valid until next setTitle call)
   * @note The returned string_view references internal storage and should not be stored long-term
   * @note This is a fast operation that does not query the system
   */
  [[nodiscard]] auto getTitle() -> std::string_view;

  /**
   * @brief Set the window title displayed in the title bar
   * @param newTitle The new title to display (will be copied)
   * 
   * Updates both the internal title storage and the system window's
   * displayed title. The change is visible immediately in the window's
   * title bar.
   *
   * @note This operation may be relatively expensive as it communicates with the window system
   * @note The title string is copied, so the parameter can be safely destroyed after the call
   */
  void setTitle(const std::string& newTitle);

  /**
   * @brief Get controlled access to implementation details
   * @return Const reference to the window implementation
   * @note This method is used internally by related classes like Renderer
   */
  [[nodiscard]] auto getImpl() const -> const WindowImpl&;

 private:
  std::string _title;
  std::unique_ptr<WindowImpl> _impl;
};

}  // namespace sdl

#endif
