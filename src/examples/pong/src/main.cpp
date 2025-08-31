#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>

#include <sdl/application.h>
#include <sdl/event.h>
#include <sdl/event_router.h>
#include <sdl/exception.h>
#include <sdl/font.h>
#include <sdl/rectangle.h>
#include <sdl/renderer.h>
#include <sdl/sdl.h>
#include <sdl/ttf_service.h>
#include <sdl/window.h>

#include "ball.h"
#include "events.h"
#include "paddle.h"
#include "player.h"
#include "pong.h"
#include "pong_ui.h"
#include "renderers.h"
#include "resources.h"
#include "timer.h"
#include "constants.h"

using namespace pong;


class PongApp : public sdl::BaseApplication {
 public:
  auto init() -> bool override {
    auto& sdl = requestSDL();
    [[maybe_unused]] auto& ttf = requestService<sdl::ttf::TTF>();
    sdl.initSubSystem(sdl::SDL::kVideo);
    sdl.initSubSystem(sdl::SDL::kEvents);

    // Validate window dimensions
    if (kWindowWidth <= 0 || kWindowHeight <= 0) {
      std::cerr << "Error: Invalid window dimensions (" << kWindowWidth << "x" << kWindowHeight << ")\n";
      return false;
    }

    if (kWindowWidth < 400 || kWindowHeight < 300) {
      std::cerr << "Warning: Window size may be too small for proper gameplay\n";
    }

    auto windowSize = Point<float>{static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight)};

    _pong = std::make_unique<Pong>(windowSize, getEventBus(), getEventRouter());
    if (!_pong) {
      std::cerr << "Error: Failed to create Pong game instance\n";
      return false;
    }

    _pongUI = std::make_unique<PongUI>(windowSize);
    if (!_pongUI) {
      std::cerr << "Error: Failed to create Pong UI instance\n";
      return false;
    }

    return true;
  }

  auto iterate() -> bool override {
    auto dt = _timer.elapsed();

    // Validate delta time to prevent physics issues
    if (dt < 0.0f || dt > 1.0f) {
      std::cerr << "Warning: Invalid delta time: " << dt << "s, clamping to safe range\n";
      dt = std::clamp(dt, 0.001f, 0.1f);
    }

    if (_pong) {
      _pong->update(dt);
    }
    
    if (_pongUI && _pong) {
      _pongUI->render(*_pong);
    }
    
    return true;
  }

  auto quit() -> void override {
    // Cleanup if necessary
  }

 private:
  std::unique_ptr<Pong> _pong = nullptr;
  std::unique_ptr<PongUI> _pongUI = nullptr;
  std::chrono::high_resolution_clock::time_point _startTime = std::chrono::high_resolution_clock::now();
  Timer _timer;
};

REGISTER_APPLICATION(PongApp);
