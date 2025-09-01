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

constexpr void validateWindowDimensions() {
  static_assert(kWindowWidth > 0 && kWindowHeight > 0, "Error: Invalid window dimensions");
  static_assert(kWindowWidth >= 400 && kWindowHeight >= 300, "Error: Window size too small for gameplay");
}

class PongApp : public sdl::BaseApplication {
 public:
  auto init() -> bool override {
    auto& sdl = requestSDL();
    [[maybe_unused]] auto& ttf = requestService<sdl::ttf::TTF>();
    sdl.initSubSystem(sdl::SDL::kVideo);
    sdl.initSubSystem(sdl::SDL::kEvents);

    validateWindowDimensions();
    auto windowSize = Point<float>{static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight)};

    _pong = std::make_unique<Pong>(windowSize, getEventBus(), getEventRouter());
    _pongUI = PongUIFactory::createPongUI(windowSize);

    return true;
  }

  auto iterate() -> bool override {
    auto dt = _timer.elapsed();

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
  Timer _timer;
};

REGISTER_APPLICATION(PongApp);
