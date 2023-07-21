#include <exception>
#include <iostream>

#include <images.h>

#include <color.h>
#include <event_processor.h>
#include <rectangle.h>
#include <renderer.h>
#include <sdl.h>
#include <texture.h>
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
      384,
      384,
      0
    };

    Renderer renderer { window, -1, { Renderer::kSoftware } };
    renderer.setRenderDrawColour(NamedColor::kMagenta);

    Texture texture { renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize() };
    renderer.setRenderDrawColour(NamedColor::kWhite);
    renderer.clear();
    texture.setTextureBlendMode(Texture::kBlend);
    renderer.copy(texture, Rectangle{ 0, 0, 384, 384 }, Rectangle{ 0, 0, 384, 384 });
    renderer.present();

    EventProcessor eventProcessor;
    eventProcessor.run();

  } catch ( std::exception &e ) {
    std::cout << "Some kind of error happened!" << std::endl;
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}
