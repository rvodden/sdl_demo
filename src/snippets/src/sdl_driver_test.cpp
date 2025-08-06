#include <SDL2/SDL.h>
#include <iostream>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    
    int num_drivers = SDL_GetNumVideoDrivers();
    std::cout << "Available video drivers:\n";
    for (int i = 0; i < num_drivers; i++) {
        std::cout << "  " << SDL_GetVideoDriver(i) << "\n";
    }
    
    std::cout << "\nCurrent video driver: " << SDL_GetCurrentVideoDriver() << "\n";
    
    SDL_Quit();
    return 0;
}