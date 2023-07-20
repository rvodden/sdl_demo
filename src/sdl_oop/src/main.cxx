#include <exception>
#include <iostream>

#include <sdl.h>
#include <color.h>
#include <window.h>
#include <renderer.h>

using namespace sdl;

int main()
{
  try {
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

  Renderer renderer { window, -1, { Renderer::kSoftware } };
  renderer.setRenderDrawColour(NamedColor::kMagenta);
  renderer.clear();
  renderer.present();

  delay(std::chrono::seconds(3));
  } catch ( std::exception &e ) {
    std::cout << "Some kind of error happened" << std::endl;
    return -1;
  }
  return 0;
}
