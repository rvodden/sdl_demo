#ifndef TICTACTOE_TICTACTOE_UI_H
#define TICTACTOE_TICTACTOE_UI_H

#include "events.h"
#include "game_constants.h"
#include "tictactoe.h"
#include <button.h>
#include <color.h>
#include <event_dispatcher.h>
#include <float_rectangle.h>
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
  TicTacToeUI(std::shared_ptr<sdlpp::EventProducer> producer,
              std::shared_ptr<sdlpp::tools::EventDispatcher> dispatcher);

  void render(const std::shared_ptr<TicTacToe>& ticTacToe);

 private:
  sdlpp::Window _window{"Tic Tac Toe", kInitialWindowWidth, kInitialWindowHeight, 0};
  std::shared_ptr<sdlpp::EventProducer> _eventProducer;
  std::shared_ptr<sdlpp::tools::EventDispatcher> _eventDispatcher;

  std::shared_ptr<sdlpp::Renderer> _renderer = std::make_shared<sdlpp::Renderer>(_window);
  std::shared_ptr<sdlpp::tools::SpriteRenderer> _spriteRenderer =
      std::make_shared<sdlpp::tools::SpriteRenderer>(_renderer);

  std::shared_ptr<sdlpp::Texture> _spriteSheet = std::make_shared<sdlpp::Texture>(
      *_renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize());
  sdlpp::tools::Sprite _board{_spriteSheet,
                {0, 0, kCellWidth* kCellColumns, kCellHeight* kCellRows}};
  sdlpp::tools::Sprite _letterO{
      _spriteSheet,
      {kCellWidth * kCellColumns, kCellHeight, kCellWidth, kCellHeight}};
  sdlpp::tools::Sprite _letterX{_spriteSheet,
                  {kCellWidth * kCellColumns, 0, kCellWidth, kCellHeight}};

  std::vector<std::unique_ptr<sdlpp::tools::Button>> buttons;

  static const uint8_t kCellWidth = 128;
  static const uint8_t kCellHeight = kCellWidth;
  static const uint16_t kInitialWindowWidth = 384;
  static const uint16_t kInitialWindowHeight = kInitialWindowWidth;
};

#endif // TICTACTOE_TICTACTOE_UI_H