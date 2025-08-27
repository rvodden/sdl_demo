#include <application.h>
#include <color.h>
#include <renderer.h>
#include <sdl.h>
#include <window.h>
#include <event.h>
#include <event_router.h>

#include <iostream>

using namespace sdl;

class MyGame : public BaseApplication {
 public:
  MyGame() : window_{"", 0, 0, 0}, renderer_{window_} {
    // Constructor implementation - actual initialization happens in init()
  }

  auto init() -> bool override {
    std::cout << "MyGame::init() called\n";
    
    try {
      sdl_.initSubSystem(SDL::kVideo);
      sdl_.initSubSystem(SDL::kEvents);
      
      window_ = Window("SDL3 Application Example", 800, 600, 0);
      renderer_ = Renderer(window_);
      
      // TODO: Event handling will be added back once ApplicationRunner
      // provides access to the event system
      
      std::cout << "Game initialized successfully\n";
      return true;
    } catch (const std::exception& e) {
      std::cout << "Failed to initialize: " << e.what() << "\n";
      return false;
    }
  }

  auto iterate() -> bool override {
    // Render a blue screen
    Color blue(0, 0, 255, 255);
    renderer_.setDrawColour(blue);
    renderer_.clear();
    renderer_.present();
    
    return true;
  }

  auto quit() -> void override {
    std::cout << "MyGame::quit() called\n";
  }

 private:
  SDL sdl_;
  Window window_;
  Renderer renderer_;
};

// Clean registration using the new macro
REGISTER_APPLICATION(MyGame)
