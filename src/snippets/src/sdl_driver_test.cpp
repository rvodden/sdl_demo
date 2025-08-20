#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

int main(int argc, char **argv) {
  auto retval = SDL_Init(SDL_INIT_VIDEO);
  if (!retval) {
    std::cerr << SDL_GetError() << "\n";
    return 1;
  }

  int num_drivers = SDL_GetNumVideoDrivers();
  std::cout << "Available video drivers:\n";
  for (int i = 0; i < num_drivers; i++) {
    std::cout << "  " << SDL_GetVideoDriver(i) << "\n";
  }

  std::cout << "\nCurrent video driver: " << SDL_GetCurrentVideoDriver()
            << "\n";

  SDL_Quit();
  return 0;
}
