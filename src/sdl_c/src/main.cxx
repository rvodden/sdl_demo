#include <iostream>
#include <cmath>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <images.h>

int main()
{
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow(
    "SDL2Test",
    384,
    384,
    0
  );
  if(window == nullptr) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    return -1;
  }

  auto renderer = SDL_CreateRenderer(window, nullptr);
  if(renderer == nullptr) {
    std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    return -1;
  }
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  auto rwops = SDL_IOFromConstMem(&_binary_tic_tac_toe_png_start, ticTacToeSize());
  if(rwops == nullptr) {
    std::cout << "Error extracting image from embedded binary: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Texture *texture = IMG_LoadTexture_IO(renderer, rwops, 1);
  if(texture == nullptr) {
    std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
    return -1;
  }
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_FRect rect = { 0, 0, 384, 384 };
  SDL_RenderTexture(renderer, texture, &rect , &rect);
  SDL_RenderPresent(renderer);

  bool quit = false;
  int bx, by, button_number;
  SDL_Event event;
  while (not quit) {        
    SDL_WaitEvent(&event);        
    
    switch(event.type)        
    {
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
        bx = static_cast <int> (std::floor(event.button.x / 128));
        by = static_cast <int> (std::floor(event.button.y / 128));
        button_number = 3 * by + bx;
        std::cout << "Got a mouse event on button number " << button_number << std::endl;
        break;
      case SDL_EVENT_QUIT:        
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
