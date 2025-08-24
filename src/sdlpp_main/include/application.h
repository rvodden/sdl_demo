#ifndef SDLPP_APPLICATION_H
#define SDLPP_APPLICATION_H

#include <functional>
#include <memory>
#include <cstdint>

#include "sdlpp_main_export.h"

// Include for event system - needed for inline method definitions
#include "event.h"
#include "event_router.h"

namespace sdlpp {

enum class AppResult : uint8_t {
  kContinue,
  kSuccess,
  kFailure
};

namespace detail {

SDLPP_MAIN_EXPORT auto run_sdl_main() -> int;

// Application factory function type
using ApplicationFactory = std::function<void()>;

// Global application factory registration
SDLPP_MAIN_EXPORT auto register_application_factory(ApplicationFactory factory) -> void;

class SDLPP_MAIN_EXPORT ApplicationRegistry {
 public:
  static auto instance() -> ApplicationRegistry&;
  
  auto set_callbacks(
    std::function<AppResult(void**, int, char**)> init_fn,
    std::function<AppResult(void*)> iterate_fn,
    std::function<void(void*, AppResult)> quit_fn,
    std::shared_ptr<tools::EventRouter> eventRouter
  ) -> void;
  
  [[nodiscard]] auto get_init_callback() const -> const std::function<AppResult(void**, int, char**)>&;
  [[nodiscard]] auto get_iterate_callback() const -> const std::function<AppResult(void*)>&;
  [[nodiscard]] auto get_quit_callback() const -> const std::function<void(void*, AppResult)>&;
  [[nodiscard]] auto get_event_router() const -> std::shared_ptr<tools::EventRouter>;

 private:
  std::function<AppResult(void**, int, char**)> init_callback_;
  std::function<AppResult(void*)> iterate_callback_;
  std::function<void(void*, AppResult)> quit_callback_;
  std::shared_ptr<tools::EventRouter> _eventRouter;
};
}  // namespace detail

template <typename Derived>
class SDLPP_MAIN_EXPORT Application {
  friend Derived;
 public:
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  auto operator=(const Application&) -> Application& = delete;
  auto operator=(Application&&) -> Application& = delete;
  virtual ~Application() = default;

  static auto run() -> int {
    return detail::run_sdl_main();
  }

 private:
  Application() {
    detail::ApplicationRegistry::instance().set_callbacks(
      [](void** appstate, int argc, char* argv[]) { return app_init(appstate, argc, argv); },
      [](void* appstate) { return app_iterate(appstate); },
      [](void* appstate, AppResult result) { app_quit(appstate, result); },
      _eventRouter
    );
  }
  

  static inline std::unique_ptr<Derived> _instance;
  std::shared_ptr<EventBus> _eventBus;
  std::shared_ptr<tools::EventRouter> _eventRouter;

  template<typename EventType, typename Callable> 
  void registerEventHandler(Callable&& callable) {
    getEventRouter()->template registerEventHandler<EventType, Callable>(std::forward<Callable>(callable));
  };

  auto getEventBus() -> std::shared_ptr<EventBus> { return _eventBus; }
  auto getEventRouter() -> std::shared_ptr<tools::EventRouter> { return _eventRouter; }
  
  static auto app_init(void** appstate, [[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> AppResult {
    if (!_instance) {
      _instance = std::make_unique<Derived>();
      
      // Initialize event system - will be implemented in the .cpp file
      _instance->initializeEventSystem();
    }
    
    *appstate = _instance.get();
    
    if (_instance->init()) {
      return AppResult::kContinue;
    }
    return AppResult::kFailure;
  }

  static auto app_iterate(void* appstate) -> AppResult {
    auto* app = static_cast<Derived*>(appstate);
    
    if (app->iterate()) {
      return AppResult::kContinue;
    }
    return AppResult::kSuccess;
  }

  static auto app_quit(void* appstate, [[maybe_unused]] AppResult result) -> void {
    auto* app = static_cast<Derived*>(appstate);
    app->quit();
    _instance.reset();
  }

  // Event system initialization and processing methods
  void initializeEventSystem() {
    _eventBus = std::make_shared<EventBus>();
    _eventRouter = std::make_shared<tools::EventRouter>(
      std::static_pointer_cast<BaseEventBus>(_eventBus));
  }

  // Method to access event system from SDL callbacks
  auto getEventBusForCallback() -> std::shared_ptr<EventBus> { return _eventBus; }
  auto getEventRouterForCallback() -> std::shared_ptr<tools::EventRouter> { return _eventRouter; }
};

}  // namespace sdlpp

#endif  // SDLPP_APPLICATION_H
