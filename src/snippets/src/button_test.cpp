#include <button.h>
#include <event.h>
#include <event_router.h>
#include <renderer.h>
#include <sdl.h>
#include <window.h>

#include <iostream>
#include <memory>

using namespace sdlpp;
using namespace sdlpp::tools;

auto main() -> int {
  try {
    SDL sdl;
    sdl.initSubSystem(SDL::kVideo);

    auto eventBus = std::make_shared<EventBus>();
    auto eventRouter = std::make_shared<EventRouter>(eventBus);

    Window window("Button Test", 400, 300, 0);  // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    auto renderer = std::make_shared<Renderer>(window);

    // Create a button at position (50, 50) with size 100x50
    auto button = std::make_unique<Button>(
        eventRouter, FloatRectangle{50.0F, 50.0F, 100.0F, 50.0F});  // NOLINT(cppcoreguidelines-avoid-magic-numbers)

    std::cout << "Created button at (50, 50) with size 100x50\n";
    std::cout << "Button should respond to clicks between x=50-150, y=50-100\n";

    // Register button event handler
    button->registerEventHandler(
        [](const MouseButtonEvent& mouseEvent) -> void {
          std::cout << "BUTTON CLICKED! Mouse at (" << mouseEvent.x << ","
                    << mouseEvent.y << ")\n";
        });

    // Register global mouse event handler for debugging
    eventRouter->registerEventHandler<MouseButtonEvent>(
        [](const MouseButtonEvent& mouseEvent) -> void {
          std::cout << "Mouse event: (" << mouseEvent.x << "," << mouseEvent.y
                    << ") button=" << static_cast<int>(mouseEvent.button)
                    << " down=" << mouseEvent.down << "\n";
        });

    // Simple render - just clear and present
    renderer->setRenderDrawColour(NamedColor::kBlack);
    renderer->clear();
    renderer->present();

    std::cout << "Click anywhere in the window to test button detection.\n";
    std::cout << "Button area is at (50-150, 50-100).\n";
    std::cout << "Press Ctrl+C to quit.\n";

    eventRouter->run();
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << "\n";
    return -1;
  }
  return 0;
}
