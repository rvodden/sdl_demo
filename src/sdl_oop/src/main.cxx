#include <exception>
#include <iostream>

#include <color.h>
#include <event_processor.h>
#include <renderer.h>
#include <sdl.h>
#include <window.h>

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

    EventProcessor eventProcessor;
    eventProcessor.run();

  } catch ( std::exception &e ) {
    std::cout << "Some kind of error happened" << std::endl;
    return -1;
  }
  return 0;
}
