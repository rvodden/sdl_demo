#include <array>
#include <cmath>
#include <exception>
#include <format>
#include <forward_list>
#include <functional>
#include <iostream>
#include <ranges>

#include <images.h>

#include <color.h>
#include <event.h>
#include <float_rectangle.h>
#include <renderer.h>
#include <sdl.h>
#include <texture.h>
#include <window.h>

#include <button.h>
#include <event_dispatcher.h>
#include <sprite.h>
#include <sprite_renderer.h>
#include <user_event.h>

using namespace sdl;
using namespace sdl::tools;


class ClickEvent: public CustomUserEvent<ClickEvent> {
  public:

    ClickEvent(
        std::chrono::duration<int64_t, std::milli> ts,
        uint32_t winId,
        uint8_t xParam,
        uint8_t yParam
    ) :  CustomUserEvent(ts, winId), x(xParam), y(yParam) {};

  uint8_t x;
  uint8_t y;
};

class GameCompletedEvent: public CustomUserEvent<GameCompletedEvent> {
  public:
    GameCompletedEvent(
        std::chrono::duration<int64_t, std::milli> ts,
        uint32_t winId,
        uint8_t xParam,
        uint8_t yParam
    ) :  CustomUserEvent(ts, winId) {};
};

class TicTacToe {
  public:
    enum class Player {
      O,
      X
    };

    enum class State { Playing, PlayerOWins, PlayerXWins, Draw };

    std::optional<Player> getCellState(const uint8_t& x, const uint8_t& y) const {
      return _cells[_index(x,y)];
    }

    void play(uint8_t x, uint8_t y) {
      const auto index = _index(x, y);
      if (_cells[index]) return;

      _cells[index] = { _turn };
      _turn = _turn == Player::O ? Player::X : Player::O;
    }

  private:
    std::array<std::optional<Player>, 9> _cells {
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt,
      std::nullopt
    };
    Player _turn { Player::O };

    static inline uint8_t _index(uint8_t x,  uint8_t y) {
      if (x > 2 || y > 2) throw std::runtime_error(std::format("Cell coordinates are out of bounds: ({},{})", x, y));
      return x + static_cast<uint8_t>(y * 3);
    }

    State _checkWinCondition() const {
          // Check rows, columns, diagonals for wins
          // Check for draw condition
          // Return appropriate GameState
    }

    State _checkRows() {
      for (uint8_t row = 0; row < 3; ++row) {
          if (_cells[_index(0, row)] &&
              _cells[_index(0, row)] == _cells[_index(1, row)] &&
              _cells[_index(1, row)] == _cells[_index(2, row)]) {
              return _cells[_index(0, row)] == Player::O ? State::PlayerOWins : State::PlayerXWins;
          }
      }
      return State::Playing;
    }

    State _checkColumns() {
      for (uint8_t col = 0; col < 3; ++col) {
          if (_cells[_index(col, 0)] &&
              _cells[_index(col, 0)] == _cells[_index(col, 1)] &&
              _cells[_index(col, 1)] == _cells[_index(col, 2)]) {
              return _cells[_index(col, 0)] == Player::O ? State::PlayerOWins : State::PlayerXWins;
          }
      }
      return State::Playing;
    }

    State _checkDiagonals() {
      // Check diagonals
      if (_cells[_index(0, 0)] &&
          _cells[_index(0, 0)] == _cells[_index(1, 1)] &&
          _cells[_index(1, 1)] == _cells[_index(2, 2)]) {
          return _cells[_index(0, 0)] == Player::O ? State::PlayerOWins : State::PlayerXWins;
      }

      if (_cells[_index(2, 0)] &&
          _cells[_index(2, 0)] == _cells[_index(1, 1)] &&
          _cells[_index(1, 1)] == _cells[_index(0, 2)]) {
          return _cells[_index(2, 0)] == Player::O ? State::PlayerOWins : State::PlayerXWins;
      }

    return State::Playing;
  }

    State _checkForDraw() {
      // Check for draw (all cells filled)
      bool allFilled = true;
      for (const auto& cell : _cells) {
          if (!cell) {
              allFilled = false;
              break;
          }
      }

      return allFilled ? State::Draw : State::Playing;
    }
};

class TicTacToeUI {
  public:
    TicTacToeUI(EventProducer& producer, EventDispatcher& dispatcher ) :
      _eventProducer(producer),
      _eventDispatcher(dispatcher)
    {
      _spriteSheet.setTextureBlendMode(Texture::kBlend);
      buttons.reserve(9);
      for( uint8_t x : std::ranges::iota_view<uint8_t, uint8_t>{ 0, 3 } ) {
        for( uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{ 0, 3 } ) {
          auto button = std::make_unique<Button>(_eventDispatcher, FloatRectangle{ static_cast<float>(x) * 128 + 1, static_cast<float>(y) * 128 + 1, 128, 128} );
          button->registerEventHandler([this, x, y](__attribute_maybe_unused__ const MouseButtonEvent& mouseButtonEvent){
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            );
            if(mouseButtonEvent.down) _eventProducer.produce(std::make_unique<ClickEvent>(now, 0, x, y));
          });
          buttons.emplace_back(std::move(button));
        }
      }
    };

    void render(const TicTacToe& ticTacToe) {
      _renderer.setRenderDrawColour(NamedColor::kWhite);
      _renderer.clear();
      
      _spriteRenderer.render(_board, 0, 0);

      for(uint8_t x : std::ranges::iota_view<uint8_t, uint8_t>{0, 3}) {
        for(uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, 3}) {
          const auto cellState = ticTacToe.getCellState(x, y);
          if(!cellState) continue;
          _spriteRenderer.render(cellState.value() == TicTacToe::Player::O ? _letterO : _letterX, static_cast<float>(x) * 128, static_cast<float>(y) * 128);
        }
      }
      _renderer.present();
    }

  private:
    Window _window {
      "SDL2Test",
      384,
      384,
      0
    };
    EventProducer& _eventProducer;
    EventDispatcher& _eventDispatcher;
    
    Renderer _renderer { _window };
    const SpriteRenderer _spriteRenderer { _renderer };
    Texture _spriteSheet { _renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize() };
    
    const Sprite _board {_spriteSheet, {0, 0, 384, 384}};
    const Sprite _letterO {_spriteSheet, {384, 128, 128, 128}};
    const Sprite _letterX {_spriteSheet, {384, 0, 128, 128}};
    
    std::vector<std::unique_ptr<Button>> buttons;

};

class GameProgressor :  public EventHandler<ClickEvent>, public BaseEventHandler {
  public:
    GameProgressor(TicTacToe& ticTacToe) : _ticTacToe(ticTacToe) {};
    void handle(const ClickEvent& clickEvent) override {
      _ticTacToe.play(clickEvent.x, clickEvent.y);
    }

  private:
    TicTacToe& _ticTacToe;
};

class GameRenderer :  public EventHandler<ClickEvent>, public BaseEventHandler {
  public:
    GameRenderer(TicTacToe& ticTacToe, TicTacToeUI& ticTacToeUI) : 
      _ticTacToe(ticTacToe),
      _ticTacToeUI(ticTacToeUI) {};

    void handle( [[maybe_unused]] const ClickEvent& clickEvent) override {
      _ticTacToeUI.render(_ticTacToe);
    }

  private:
    TicTacToe& _ticTacToe;
    TicTacToeUI& _ticTacToeUI;
};

int main()
{
  try {
    SDL sdl;
    sdl.initSubSystem(SDL::kVideo);

    EventProducer eventProducer { }; 
    EventDispatcher eventDispatcher { eventProducer };
    
    TicTacToe ticTacToe {};
    TicTacToeUI ticTacToeUI { eventProducer, eventDispatcher };

    ticTacToeUI.render(ticTacToe);

    GameProgressor gameProgressor(ticTacToe);
    GameRenderer gameRenderer(ticTacToe, ticTacToeUI);

    eventDispatcher.registerEventHandler(gameRenderer);
    eventDispatcher.registerEventHandler(gameProgressor);

    eventDispatcher.run();

  } catch ( std::exception &e ) {
    std::cout << "Some kind of error happened!" << std::endl;
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}
