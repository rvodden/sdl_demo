#ifndef SDL_WINDOW_IMPL_H
#define SDL_WINDOW_IMPL_H

#include <SDL3/SDL.h>

namespace sdl {

class Window;
class Renderer;
class RendererImpl;

class WindowImpl {
  friend Window;
  friend RendererImpl;
  friend class MessageBoxImpl;

public:
  [[nodiscard]] auto getSdlWindow() const -> SDL_Window* { return _sdlWindow; }

 private:
  SDL_Window* _sdlWindow{nullptr};
};

}  // namespace sdl

#endif
