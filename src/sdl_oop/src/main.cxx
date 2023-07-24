#include <exception>
#include <forward_list>
#include <functional>
#include <iostream>
#include <ranges>

#include <images.h>

#include <color.h>
#include <event.h>
#include <rectangle.h>
#include <renderer.h>
#include <sdl.h>
#include <texture.h>
#include <window.h>

#include <button.h>
#include <event_dispatcher.h>
#include <sprite.h>
#include <sprite_renderer.h>

using namespace sdl;
using namespace sdl::tools;

static void mouseButtonEventHandler(uint8_t number, [[maybe_unused]] const MousePositionEvent& mousePositionEvent) {
  std::cout << "Got a mouse button event : " << +number << std::endl;
}

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
    SpriteRenderer spriteRenderer( renderer );

    Texture texture { renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize() };
    const Sprite board {texture, {0, 0, 384, 384}};

    renderer.setRenderDrawColour(NamedColor::kWhite);
    renderer.clear();
    texture.setTextureBlendMode(Texture::kBlend);
    
    spriteRenderer.render(board, 0, 0);
    renderer.present();

    EventProducer eventProducer { }; 
    EventDispatcher eventDispatcher { eventProducer };
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.reserve(9);
    for( uint32_t i : std::ranges::iota_view{ 0, 3 } ) {
      for( uint32_t j : std::ranges::iota_view{ 0, 3 } ) {
        auto button = std::make_unique<Button>(eventDispatcher, Rectangle{ j * 128 + 1, i * 128 + 1, 128u, 128u} );
        button->registerEventHandler(std::bind(mouseButtonEventHandler, 3*i + j, std::placeholders::_1));
        buttons.emplace_back(std::move(button));
      }
    }
    eventDispatcher.run();

  } catch ( std::exception &e ) {
    std::cout << "Some kind of error happened!" << std::endl;
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}
