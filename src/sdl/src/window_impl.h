#ifndef __SDL_WINDOW_IMPL_H__
#define __SDL_WINDOW_IMPL_H__

#include <SDL2/SDL.h>

namespace sdl {

class Window;
class Renderer;

class WindowImpl {
  friend Window;
  friend Renderer;

  private:
    SDL_Window *_sdlWindow { nullptr };
};

}

#endif
