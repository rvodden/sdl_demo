#include "sdl_main_export.h"
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1 //NOLINT(cppcoreguidelines-macro-usage)
#define SDL_MAIN_HANDLED 1 //NOLINT(cppcoreguidelines-macro-usage)
#include <SDL3/SDL_main.h>

// Forward declarations for SDL callbacks
extern "C" {
  auto SDL_AppInit(void** appstate, int argc, char* argv[]) -> SDL_AppResult;
  auto SDL_AppIterate(void* appstate) -> SDL_AppResult;
  auto SDL_AppEvent(void* appstate, SDL_Event* event) -> SDL_AppResult;
  auto SDL_AppQuit(void* appstate, SDL_AppResult result) -> void;
}

// Undefine main macro to provide actual main function
#ifdef main
#undef main
#endif

extern "C" {
// Provide main() function when using callbacks
// SDL3 doesn't automatically provide main() for shared libraries with callbacks
SDL_MAIN_EXPORT int main(int argc, char* argv[]) {
  return SDL_EnterAppMainCallbacks(argc, argv, SDL_AppInit, SDL_AppIterate, SDL_AppEvent, SDL_AppQuit);
}
}
