#include <application.h>
#include <color.h>
#include <renderer.h>
#include <sdl.h>
#include <window.h>
#include <event.h>
#include <event_router.h>

#include <SDL3/SDL_main.h>
#include <iostream>

#define SDL_MAIN_USE_CALLBACKS 1

using namespace sdlpp;

class MyGame : public Application<MyGame> {
  friend Application<MyGame>;

 public:
  MyGame() : window_{"", 0, 0, 0}, renderer_{window_} {
    // Constructor implementation - actual initialization happens in init()
  }

 protected:

 private:
  SDL sdl_;
  Window window_;
  Renderer renderer_;
  bool _shouldExit = false;

  auto init() -> bool {
    std::cout << "MyGame::init() called\n";
    
    try {
      sdl_.initSubSystem(SDL::kVideo);
      sdl_.initSubSystem(SDL::kEvents);
      
      window_ = Window("SDL3 Application Example", 800, 600, 0);
      renderer_ = Renderer(window_);
    
      registerEventHandler<QuitEvent>(
      [this](const QuitEvent&) {
        std::cout << "Quit event received - exiting gracefully\n";
        _shouldExit = true;
      });

      // Register mouse button event handler as an example
      registerEventHandler<MouseButtonEvent>(
      [this](const MouseButtonEvent& e) {
        if (e.down) {
          std::cout << "Mouse button " << static_cast<int>(e.button) 
                   << " pressed at (" << e.x << ", " << e.y << ")\n";
        }
      });
      
      std::cout << "Game initialized successfully\n";
      return true;
    } catch (const std::exception& e) {
      std::cout << "Failed to initialize: " << e.what() << "\n";
      return false;
    }
  }

  auto iterate() -> bool {
    // Check if we should exit due to quit event
    if (_shouldExit) {
      return false;
    }

    // Render a blue screen
    Color blue(0, 0, 255, 255);
    renderer_.setRenderDrawColour(blue);
    renderer_.clear();
    renderer_.present();
    
    return true;
  }

  auto quit() -> void {
    std::cout << "MyGame::quit() called\n";
  }
};

// Explicit template instantiation
template class sdlpp::Application<MyGame>;

// Register the application factory
namespace {
  struct AppRegistrar {
    AppRegistrar() {
      sdlpp::detail::register_application_factory([]() {
        static MyGame app;
        (void)app;  // Trigger callback registration via constructor
      });
    }
  };
  static const AppRegistrar registrar;
}
