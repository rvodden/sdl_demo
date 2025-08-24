#include "application.h"
#include "constexpr_map.h"
#include "event.h"
#include "event_router.h"

#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1 //NOLINT(cppcoreguidelines-macro-usage)
#define SDL_MAIN_HANDLED 1 //NOLINT(cppcoreguidelines-macro-usage)
#include <SDL3/SDL_main.h>
#include <array>
#include <utility>


namespace sdlpp {

namespace {
  constexpr auto app_result_array = std::array{
      std::pair{AppResult::kContinue, SDL_APP_CONTINUE},
      std::pair{AppResult::kSuccess, SDL_APP_SUCCESS},
      std::pair{AppResult::kFailure, SDL_APP_FAILURE}
  };
  constexpr auto app_result_map = vodden::Map(app_result_array);

  constexpr auto sdl_result_array = std::array{
      std::pair{SDL_APP_CONTINUE, AppResult::kContinue},
      std::pair{SDL_APP_SUCCESS, AppResult::kSuccess},
      std::pair{SDL_APP_FAILURE, AppResult::kFailure}
  };
  constexpr auto sdl_result_map = vodden::Map(sdl_result_array);
}  // namespace

namespace detail {

static ApplicationFactory global_application_factory;

auto register_application_factory(ApplicationFactory factory) -> void {
  global_application_factory = std::move(factory);
}

auto ApplicationRegistry::instance() -> ApplicationRegistry& {
  static ApplicationRegistry registry;
  return registry;
}

auto ApplicationRegistry::set_callbacks(
  std::function<AppResult(void**, int, char**)> init_fn,
  std::function<AppResult(void*)> iterate_fn,
  std::function<void(void*, AppResult)> quit_fn,
  std::shared_ptr<tools::EventRouter> eventRouter
) -> void {
  init_callback_ = std::move(init_fn);
  iterate_callback_ = std::move(iterate_fn);
  quit_callback_ = std::move(quit_fn);
  _eventRouter = std::move(eventRouter);
}

auto ApplicationRegistry::get_init_callback() const -> const std::function<AppResult(void**, int, char**)>& {
  return init_callback_;
}

auto ApplicationRegistry::get_iterate_callback() const -> const std::function<AppResult(void*)>& {
  return iterate_callback_;
}

auto ApplicationRegistry::get_quit_callback() const -> const std::function<void(void*, AppResult)>& {
  return quit_callback_;
}

auto run_sdl_main() -> int {
  // When using SDL_MAIN_USE_CALLBACKS, SDL3 automatically provides main()
  // This function should not be called directly
  return 0;
}

}  // namespace detail
}  // namespace sdlpp

extern "C" {

SDLPP_MAIN_EXPORT auto SDL_AppInit(void** appstate, int argc, char* argv[]) -> SDL_AppResult {
  const auto& callback = sdlpp::detail::ApplicationRegistry::instance().get_init_callback();
  if (callback) {
    const auto result = callback(appstate, argc, argv);
    return sdlpp::app_result_map[result];
  }
  return SDL_APP_FAILURE;
}

SDLPP_MAIN_EXPORT auto SDL_AppIterate(void* appstate) -> SDL_AppResult {
  const auto& callback = sdlpp::detail::ApplicationRegistry::instance().get_iterate_callback();
  if (callback) {
    const auto result = callback(appstate);
    return sdlpp::app_result_map[result];
  }
  return SDL_APP_SUCCESS;
}

SDLPP_MAIN_EXPORT auto SDL_AppQuit(void* appstate, SDL_AppResult result) -> void {
  const auto& callback = sdlpp::detail::ApplicationRegistry::instance().get_quit_callback();
  if (callback) {
    callback(appstate, sdlpp::sdl_result_map[result]);
  }
}

SDLPP_MAIN_EXPORT auto SDL_AppEvent([[maybe_unused]] void* appstate, SDL_Event* event) -> SDL_AppResult {
  // Handle quit events directly
  if (event != nullptr && event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // This will cause the app to quit
  }
  
  return SDL_APP_CONTINUE;
}

}


// Undefine main macro to provide actual main function
#ifdef main
#undef main
#endif

extern "C" {
// Provide main() function when using callbacks
// SDL3 doesn't automatically provide main() for shared libraries with callbacks
SDLPP_MAIN_EXPORT int main(int argc, char* argv[]) {
  // Call the application factory to trigger callback registration
  if (sdlpp::detail::global_application_factory) {
    sdlpp::detail::global_application_factory();
  }
  
  return SDL_EnterAppMainCallbacks(argc, argv, SDL_AppInit, SDL_AppIterate, SDL_AppEvent, SDL_AppQuit);
}

}
