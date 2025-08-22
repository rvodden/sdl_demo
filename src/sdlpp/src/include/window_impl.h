#ifndef SDL_WINDOW_IMPL_H
#define SDL_WINDOW_IMPL_H

#include <SDL3/SDL.h>

namespace sdlpp {

class Window;
class Renderer;

class WindowImpl {
  friend Window;
  friend Renderer;

 private:
  SDL_Window* _sdlWindow{nullptr};
};

}  // namespace sdlpp

#endif
