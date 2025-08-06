#include <exception>
#include <forward_list>
#include <functional>
#include <iostream>
#include <ranges>

#include <images.h>

#include <color.h>
#include <event.h>
#include <float_rectangle.h>
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

static void mouseButtonEventHandler(
  const SpriteRenderer& spriteRenderer,
  const Sprite& sprite,
  const MousePositionEvent& mousePositionEvent ) {
  std::cout << "Got a mouse button event." << std::endl;
  spriteRenderer.render(sprite, (mousePositionEvent.x / 128) * 128, (mousePositionEvent.y / 128) * 128);
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
    const Sprite letterO {texture, {384, 128, 128, 128}};
    const Sprite letterX {texture, {384, 0, 128, 128}};

    renderer.setRenderDrawColour(NamedColor::kWhite);
    renderer.clear();
    texture.setTextureBlendMode(Texture::kBlend);
    
    spriteRenderer.render(board, 0, 0);
    renderer.present();

    EventProducer eventProducer { }; 
    EventDispatcher eventDispatcher { eventProducer };
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.reserve(9);
    [[maybe_unused]] bool crossesTurn = false;
    for( int32_t i : std::ranges::iota_view{ 0, 3 } ) {
      for( int32_t j : std::ranges::iota_view{ 0, 3 } ) {
        auto button = std::make_unique<Button>(eventDispatcher, FloatRectangle{ j * 128 + 1, i * 128 + 1, 128, 128} );
        button->registerEventHandler([&spriteRenderer, &letterO](const MousePositionEvent& mousePositionEvent){ 
          mouseButtonEventHandler(spriteRenderer, letterO, mousePositionEvent);
        });
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
