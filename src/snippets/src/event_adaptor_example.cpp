/**
 * @brief Example demonstrating the decoupled EventAdaptor architecture
 *
 * This example shows how the new template-based EventAdaptor system works:
 * 1. EventAdaptor converts platform events to sdlpp events (zero-cost abstraction)
 * 2. TemplatedEventBus uses EventAdaptor and routes via callback
 * 3. EventRouter provides routeEvent method for callback-based usage
 * 4. Complete decoupling allows non-blocking event processing
 */

#include <iostream>
#include <memory>
#include <SDL3/SDL.h>

#include "event.h"
#include "event_router.h"

using namespace sdlpp;
using namespace sdlpp::tools;

/**
 * @brief Example event handler for demonstration
 */
class ExampleEventHandler : public EventHandler<MouseButtonEvent>,
                           public BaseEventHandler {
 public:
  void handle(const MouseButtonEvent& event) override {
    std::cout << "Mouse " 
              << (event.down ? "pressed" : "released") 
              << " at (" << event.x << ", " << event.y << ")\n";
  }
};

/**
 * @brief Demonstrate the new decoupled architecture
 */
auto main() -> int {
  std::cout << "EventAdaptor Decoupling Example\n";
  std::cout << "==============================\n\n";

  // Initialize SDL for event system
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << "\n";
    return 1;
  }

  try {
    // 1. Create EventRouter (no longer coupled to EventBus)
    std::cout << "1. Creating EventRouter (decoupled from EventBus)...\n";
    EventRouter router(nullptr);  // No EventBus dependency!
    
    // 2. Register event handler
    std::cout << "2. Registering event handler...\n";
    ExampleEventHandler handler;
    router.registerEventHandler(handler);
    
    // 3. Create CRTP-based SDL event bus (zero virtual function overhead!)
    std::cout << "3. Creating CRTP-based SDLEventBus (zero-cost abstraction)...\n";
    auto sdlEventBus = createSDLEventBus();
    
    // 4. Connect EventBus to EventRouter via callback
    std::cout << "4. Setting up callback-based routing...\n";
    sdlEventBus->setRouteCallback(
      [&router](std::unique_ptr<BaseEvent> event) {
        router.routeEvent(std::move(event));
      });
    
    // 5. Simulate SDL event processing (would normally be in SDL callback)
    std::cout << "5. Simulating SDL event processing...\n";
    SDL_Event sdlEvent;
    
    // Simulate mouse button press
    sdlEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    sdlEvent.button.windowID = 1;
    sdlEvent.button.which = 1;
    sdlEvent.button.x = 100.0f;
    sdlEvent.button.y = 200.0f;
    sdlEvent.button.button = SDL_BUTTON_LEFT;
    sdlEvent.button.down = true;
    sdlEvent.button.clicks = 1;
    
    // Process event through the new architecture
    std::cout << "   Processing mouse button press...\n";
    sdlEventBus->handlePlatformEvent(sdlEvent);
    
    // Simulate mouse button release
    sdlEvent.button.down = false;
    std::cout << "   Processing mouse button release...\n";
    sdlEventBus->handlePlatformEvent(sdlEvent);
    
    std::cout << "\n✓ Success! EventBus and EventRouter are now decoupled.\n";
    std::cout << "✓ EventRouter.routeEvent() can be used in SDL_AppEvent callbacks.\n";
    std::cout << "✓ Template-based EventAdaptor provides zero-cost abstraction.\n";

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Quit();
  return 0;
}