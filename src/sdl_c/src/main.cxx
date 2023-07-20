#include <iostream>

#include <SDL2/SDL.h>
#include <SDL_image.h>


int main()
{
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow(
    "SDL2Test",
    100,
    100,
    640,
    480,
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

  SDL_Surface *image = IMG_Load("/workspaces/test_project/data/TicTacToe.png");
  if(image == nullptr) {
    std::cout << "Error loading image: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
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

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  return 0;
}
