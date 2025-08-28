#include "application.h"
#include <constexpr_map.h>
#include "event.h"
#include "event_impl.h"
#include "event_router.h"
#include "sdl.h"

#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1 //NOLINT(cppcoreguidelines-macro-usage)
#define SDL_MAIN_HANDLED 1 //NOLINT(cppcoreguidelines-macro-usage)
#include <SDL3/SDL_main.h>
#include <array>
#include <utility>


namespace sdl {

namespace {
  constexpr auto kAppResultArray = std::array{
      std::pair{AppResult::kContinue, SDL_APP_CONTINUE},
      std::pair{AppResult::kSuccess, SDL_APP_SUCCESS},
      std::pair{AppResult::kFailure, SDL_APP_FAILURE}
  };
  constexpr auto kAppResultMap = sdl::Map(kAppResultArray);

  constexpr auto kSdlResultArray = std::array{
      std::pair{SDL_APP_CONTINUE, AppResult::kContinue},
      std::pair{SDL_APP_SUCCESS, AppResult::kSuccess},
      std::pair{SDL_APP_FAILURE, AppResult::kFailure}
  };
  constexpr auto kSdlResultMap = sdl::Map(kSdlResultArray);
}  // namespace

// ApplicationRunner implementation
auto ApplicationRunner::registerApplication(std::unique_ptr<BaseApplication> app) -> void {
  getInstance()._application = std::move(app);
}

auto ApplicationRunner::run() -> int {
  return 0; // SDL will call our callbacks
}

auto ApplicationRunner::getInstance() -> ApplicationRunner& {
  static ApplicationRunner instance;
  return instance;
}

auto ApplicationRunner::getApplication() -> BaseApplication* {
  return _application.get();
}

auto ApplicationRunner::getEventBus() -> std::shared_ptr<BaseEventBus> {
  return _eventBus;
}

auto ApplicationRunner::getEventRouter() -> std::shared_ptr<tools::EventRouter> {
  return _eventRouter;
}

auto ApplicationRunner::initializeEventSystem() -> void {
  _eventBus = createSDLEventBus();
  _eventRouter = std::make_shared<tools::EventRouter>(_eventBus);
  
  // Connect EventBus to EventRouter via callback for platform events
  _eventBus->setRouteCallback(
    [this](std::unique_ptr<BaseEvent> event) -> void {
      if (_eventRouter) {
        _eventRouter->routeEvent(std::move(event));
      }
    });
}

auto ApplicationRunner::reset() -> void {
  _application.reset();
  _eventBus.reset();
  _eventRouter.reset();
  _sdl.reset();
  _services.clear();
}

// Service management implementation
auto ApplicationRunner::getOrCreateSDL() -> SDL& {
  if (!_sdl) {
    _sdl = std::make_unique<SDL>();
  }
  return *_sdl;
}

// BaseApplication service request implementation
auto BaseApplication::requestSDL() -> SDL& {
  return ApplicationRunner::getInstance().getOrCreateSDL();
}

}  // namespace sdl

extern "C" {

SDL_APPLICATION_EXPORT auto SDL_AppInit(void** appstate, [[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> SDL_AppResult {
  auto& runner = sdl::ApplicationRunner::getInstance();
  auto* app = runner.getApplication();
  
  if (app == nullptr) {
    return SDL_APP_FAILURE;
  }
  
  // Initialize event system (fixes the encapsulation issue)
  runner.initializeEventSystem();
  
  // Store runner reference in appstate
  *appstate = &runner;
  
  if (app->init()) {
    return SDL_APP_CONTINUE;
  }
  return SDL_APP_FAILURE;
}

SDL_APPLICATION_EXPORT auto SDL_AppIterate(void* appstate) -> SDL_AppResult {
  auto* runner = static_cast<sdl::ApplicationRunner*>(appstate);
  if (runner == nullptr) {
    return SDL_APP_SUCCESS;
  }
  
  auto* app = runner->getApplication();
  if (app == nullptr) {
    return SDL_APP_SUCCESS;
  }
  
  if (app->iterate()) {
    return SDL_APP_CONTINUE;
  }
  return SDL_APP_SUCCESS;
}

SDL_APPLICATION_EXPORT auto SDL_AppQuit(void* appstate, [[maybe_unused]] SDL_AppResult result) -> void {
  auto* runner = static_cast<sdl::ApplicationRunner*>(appstate);
  if (runner == nullptr) {
    return;
  }
  
  auto* app = runner->getApplication();
  if (app != nullptr) {
    app->quit();
  }
  
  // Reset application state
  auto& instance = sdl::ApplicationRunner::getInstance();
  instance.reset();
}

SDL_APPLICATION_EXPORT auto SDL_AppEvent([[maybe_unused]] void* appstate, SDL_Event* event) -> SDL_AppResult {
  if (event == nullptr) {
    return SDL_APP_CONTINUE;
  }
  
  // Route events through EventBus platform infrastructure
  auto& instance = sdl::ApplicationRunner::getInstance();
  if (auto eventBus = instance.getEventBus()) {
    // Use the type-safe injectEvent method - no casting needed!
    eventBus->injectEvent(*event, std::type_index(typeid(SDL_Event)));
  }
  
  // Handle quit events directly
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // This will cause the app to quit
  }
  
  return SDL_APP_CONTINUE;
}

}
