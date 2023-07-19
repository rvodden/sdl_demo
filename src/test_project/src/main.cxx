#include <SDL2/SDL.h>

#include <sdl.h>
#include <color.h>
#include <window.h>
#include <renderer.h>

using namespace sdl;

int main()
{
  SDL sdl;
  sdl.initSubSystem(SDL::kVideo);

  Window window {
    "SDL2Test",
    100,
    100,
    640,
    480,
    0
  };

  Renderer renderer { window, -1, SDL_RENDERER_SOFTWARE };
  renderer.setRenderDrawColour(NamedColor::kMagenta);
  renderer.clear();
  renderer.present();

  delay(std::chrono::seconds(3));
  return 0;
}
