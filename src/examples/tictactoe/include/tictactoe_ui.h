#ifndef TICTACTOE_TICTACTOE_UI_H
#define TICTACTOE_TICTACTOE_UI_H

#include "events.h"
#include "game_constants.h"
#include "tictactoe.h"
#include <button.h>
#include <color.h>
#include <event_router.h>
#include <rectangle.h>
#include <images.h>
#include <renderer.h>
#include <sprite.h>
#include <sprite_renderer.h>
#include <texture.h>
#include <window.h>

#include <memory>
#include <vector>

class TicTacToeUI {
 public:
  TicTacToeUI(std::shared_ptr<sdl::BaseEventBus> bus,
              std::shared_ptr<sdl::tools::EventRouter> router);

  void render(const std::shared_ptr<TicTacToe>& ticTacToe);

 private:
  sdl::Window _window{"Tic Tac Toe", kInitialWindowWidth, kInitialWindowHeight, 0};
  std::shared_ptr<sdl::BaseEventBus> _eventBus;
  std::shared_ptr<sdl::tools::EventRouter> _eventRouter;

  std::shared_ptr<sdl::Renderer> _renderer = std::make_shared<sdl::Renderer>(_window);
  std::shared_ptr<sdl::tools::SpriteRenderer> _spriteRenderer =
      std::make_shared<sdl::tools::SpriteRenderer>(_renderer);

  std::shared_ptr<sdl::Texture> _spriteSheet = std::make_shared<sdl::Texture>(
      *_renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize());
  sdl::tools::Sprite _board{_spriteSheet,
                {0, 0, kCellWidth* kCellColumns, kCellHeight* kCellRows}};
  sdl::tools::Sprite _letterO{
      _spriteSheet,
      {kCellWidth * kCellColumns, kCellHeight, kCellWidth, kCellHeight}};
  sdl::tools::Sprite _letterX{_spriteSheet,
                  {kCellWidth * kCellColumns, 0, kCellWidth, kCellHeight}};

  std::vector<std::unique_ptr<sdl::tools::Button>> _buttons;

  static const uint8_t kCellWidth = 128;
  static const uint8_t kCellHeight = kCellWidth;
  static const uint16_t kInitialWindowWidth = 384;
  static const uint16_t kInitialWindowHeight = kInitialWindowWidth;
};

#endif // TICTACTOE_TICTACTOE_UI_H