#include <string_view>

#include <assert.h>

#include <sdl/application.h>
#include <sdl/font.h>
#include <sdl/renderer.h>
#include <sdl/sdl.h>
#include <sdl/text.h>
#include <sdl/ttf.h>
#include <sdl/ttf_service.h>  // Automatically registers TTF service
#include <sdl/window.h>

#include "tiny_ttf.h"

using namespace sdl;
using namespace sdl::ttf;
using namespace std::literals;

const uint32_t kScreenWidth = 800;
const uint32_t kScreenHeight = 600;
const float kScale = 4.0;

class HelloWorld : public BaseApplication {
 public:
  auto init() -> bool override {
    // Use service registry pattern - no manual object management
    auto& sdl = requestSDL();
    [[maybe_unused]] auto& ttf = requestService<TTF>();  // TTF registered automatically when linked
    sdl.initSubSystem(SDL::kVideo);
    sdl.initSubSystem(SDL::kEvents);

    _window = std::make_unique<Window>("Hello, World!", kScreenWidth, kScreenHeight, 0);
    _renderer = std::make_unique<Renderer>(*_window);

    const float fontSize = 18.0F;
    auto font = Font(tiny_ttf.data(), tiny_ttf.size(), fontSize);
    auto text = Text::renderBlended(font, "Hello, World!", NamedColor::kWhite);
    _texture = std::make_unique<Texture>(*_renderer, text);

    return true;
  }

  auto iterate() -> bool override { 
    auto outputSize = _renderer->getOutputSize();
    _renderer->setScale(kScale, kScale);
    auto textureSize = _texture->getSize();
    _renderer->setDrawColour(NamedColor::kBlack);
    _renderer->clear();

    auto renderSize = Rectangle<float>{
      (static_cast<float>(outputSize.getWidth()) / kScale - textureSize.getWidth()) / 2, 
      (static_cast<float>(outputSize.getHeight()) / kScale - textureSize.getHeight()) / 2,
      textureSize.getWidth(),
      textureSize.getHeight()
    };
    _renderer->copy(*_texture, renderSize);

    _renderer->present();

    return true; 
  }

  void quit() override {}

 private:
  // SDL and TTF now managed by framework - only app-specific objects remain
  std::unique_ptr<Window> _window  = nullptr;
  std::unique_ptr<Renderer> _renderer = nullptr;
  std::unique_ptr<Texture> _texture = nullptr;
};

REGISTER_APPLICATION(HelloWorld)

