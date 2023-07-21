#include <iostream>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <images.h>

int main()
{
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow(
    "SDL2Test",
    100,
    100,
    448,
    461,
    0
  );
  if(window == nullptr) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    return -1;
  }

  auto renderer = SDL_CreateRenderer( window, -1, 0 );
  if(renderer == nullptr) {
    std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    return -1;
  }
  SDL_SetRenderDrawColor(renderer, 0xc2, 0x00, 0x78, SDL_ALPHA_OPAQUE);
  std::cout << ticTacToeSize() << std::endl;
  auto rwops = SDL_RWFromConstMem(&_binary_tic_tac_toe_png_start, ticTacToeSize());
  if(rwops == nullptr) {
    std::cout << "Error extracting image from embedded binary: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rwops, 1);
  if(texture == nullptr) {
    std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  bool quit = false;
  SDL_Event event;
  while (not quit) {        
    SDL_WaitEvent(&event);        
    
    switch(event.type)        
    {        
      case SDL_QUIT:        
        quit = true;        
        break;        
    }        
  }   

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  return 0;
}
