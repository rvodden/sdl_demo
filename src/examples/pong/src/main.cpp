#include <chrono>
#include <memory>
#include <map>
#include <iostream>

#include <sdl/application.h>
#include <sdl/rectangle.h>
#include <sdl/renderer.h>
#include <sdl/sdl.h>
#include <sdl/ttf_service.h>
#include <sdl/event_router.h>
#include <sdl/event.h>
#include <sdl/font.h>
#include <sdl/window.h>

#include "ball.h"
#include "events.h"
#include "paddle.h"
#include "player.h"
#include "pong.h"
#include "pong_ui.h"
#include "renderers.h"
#include "resources.h"

const auto kWindowWidth = 1280;
const auto kWindowHeight = 720;

class PongApp : public sdl::BaseApplication {
 public:
  auto init() -> bool override {
    auto& sdl = requestSDL();
    [[maybe_unused]] auto& ttf = requestService<sdl::ttf::TTF>();
    sdl.initSubSystem(sdl::SDL::kVideo);
    sdl.initSubSystem(sdl::SDL::kEvents);

    auto windowSize = Point<float>{kWindowWidth, kWindowHeight};

    _pong = std::make_unique<Pong>(windowSize, getEventBus(), getEventRouter());
    _pongUI = std::make_unique<PongUI>(windowSize);

    return true;
  }

  auto iterate() -> bool override {
    // Get Time
    auto stopTime = std::chrono::high_resolution_clock::now();
	  auto dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
   
    // Update Actors
    _pong->update(dt);
    _pongUI->update(*_pong);

    // Update timer
    startTime = stopTime;

    // Render
    _pongUI->render(*_pong);
    return true;
  }

  auto quit() -> void override {
    // Cleanup if necessary
  }
 
 private:
  std::unique_ptr<Pong> _pong = nullptr;
  std::unique_ptr<PongUI> _pongUI = nullptr;
  std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
};

REGISTER_APPLICATION(PongApp);
