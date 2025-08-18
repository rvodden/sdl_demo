#include <button.h>
#include <color.h>
#include <event.h>
#include <event_dispatcher.h>
#include <float_rectangle.h>
#include <images.h>
#include <renderer.h>
#include <sdl.h>
#include <sprite.h>
#include <sprite_renderer.h>
#include <texture.h>
#include <user_event.h>
#include <window.h>

#include <array>
#include <cmath>
#include <exception>
#include <format>
#include <forward_list>
#include <functional>
#include <iostream>
#include <ranges>

using namespace sdl;
using namespace sdl::tools;

static const uint8_t kCellColumns = 3;
static const uint8_t kCellRows = 3;

class ClickEvent : public CustomUserEvent<ClickEvent> {
 public:
  ClickEvent(std::chrono::duration<int64_t, std::milli> ts, uint32_t winId,
             uint8_t xParam, uint8_t yParam)
      : CustomUserEvent(ts, winId), x(xParam), y(yParam) {};

  uint8_t x;
  uint8_t y;
};

class GameCompletedEvent : public CustomUserEvent<GameCompletedEvent> {
 public:
  GameCompletedEvent(std::chrono::duration<int64_t, std::milli> ts,
                     uint32_t winId)
      : CustomUserEvent(ts, winId) {};
};

class TicTacToe {
 public:
  enum class Player : std::uint8_t { O, X };

  enum class State : std::uint8_t { Playing, PlayerOWins, PlayerXWins, Draw };

  [[nodiscard]] auto getCellState(const uint8_t& x, const uint8_t& y) const
      -> std::optional<Player> {
    return _cells.at(_index(x, y));
  }

  void play(uint8_t x, uint8_t y) {
    const auto index = _index(x, y);
    if (_cells.at(index)) {
      return;
    }

    _cells.at(index) = {_turn};
    _turn = _turn == Player::O ? Player::X : Player::O;
  }

 private:
  std::array<std::optional<Player>,
             static_cast<std::size_t>(kCellColumns* kCellRows)>
      _cells{std::nullopt, std::nullopt, std::nullopt,
             std::nullopt, std::nullopt, std::nullopt,
             std::nullopt, std::nullopt, std::nullopt};
  Player _turn{Player::O};

  static auto _index(uint8_t x, uint8_t y) -> uint8_t {
    if (x > 2 || y > 2) {
      throw std::runtime_error(
          std::format("Cell coordinates are out of bounds: ({},{})", x, y));
    }
    return x + static_cast<uint8_t>(y * 3);
  }

  [[nodiscard]] auto _checkWinCondition() const -> State {
    State state = _checkRows();
    if (state != State::Playing) {
      return state;
    }

    state = _checkColumns();
    if (state != State::Playing) {
      return state;
    }

    state = _checkDiagonals();
    if (state != State::Playing) {
      return state;
    }

    state = _checkForDraw();
    if (state != State::Playing) {
      return state;
    }

    return State::Playing;
  }

  [[nodiscard]] auto _checkRows() const -> State {
    for (uint8_t row = 0; row < 3; ++row) {
      if (_cells.at(_index(0, row)) &&
          _cells.at(_index(0, row)) == _cells.at(_index(1, row)) &&
          _cells.at(_index(1, row)) == _cells.at(_index(2, row))) {
        return _cells.at(_index(0, row)) == Player::O ? State::PlayerOWins
                                                      : State::PlayerXWins;
      }
    }
    return State::Playing;
  }

  [[nodiscard]] auto _checkColumns() const -> State {
    for (uint8_t col = 0; col < 3; ++col) {
      if (_cells.at(_index(col, 0)) &&
          _cells.at(_index(col, 0)) == _cells.at(_index(col, 1)) &&
          _cells.at(_index(col, 1)) == _cells.at(_index(col, 2))) {
        return _cells.at(_index(col, 0)) == Player::O ? State::PlayerOWins
                                                      : State::PlayerXWins;
      }
    }
    return State::Playing;
  }

  [[nodiscard]] auto _checkDiagonals() const -> State {
    // Check diagonals
    if (_cells.at(_index(0, 0)) &&
        _cells.at(_index(0, 0)) == _cells.at(_index(1, 1)) &&
        _cells.at(_index(1, 1)) == _cells.at(_index(2, 2))) {
      return _cells.at(_index(0, 0)) == Player::O ? State::PlayerOWins
                                                  : State::PlayerXWins;
    }

    if (_cells.at(_index(2, 0)) &&
        _cells.at(_index(2, 0)) == _cells.at(_index(1, 1)) &&
        _cells.at(_index(1, 1)) == _cells.at(_index(0, 2))) {
      return _cells.at(_index(2, 0)) == Player::O ? State::PlayerOWins
                                                  : State::PlayerXWins;
    }

    return State::Playing;
  }

  [[nodiscard]] auto _checkForDraw() const -> State {
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
  TicTacToeUI(std::shared_ptr<EventProducer> producer,
              std::shared_ptr<EventDispatcher> dispatcher)
      : _eventProducer(std::move(producer)),
        _eventDispatcher(std::move(dispatcher)) {
    _spriteSheet->setTextureBlendMode(Texture::kBlend);
    buttons.reserve(static_cast<std::size_t>(kCellColumns * kCellRows));
    for (uint8_t x :
         std::ranges::iota_view<uint8_t, uint8_t>{0, kCellColumns}) {
      for (uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellRows}) {
        float buttonX = (static_cast<float>(x) * kCellWidth) + 1;
        float buttonY = (static_cast<float>(y) * kCellHeight) + 1;
        std::cout << "Creating button at grid (" << static_cast<int>(x) << ","
                  << static_cast<int>(y) << ") -> screen (" << buttonX << ","
                  << buttonY << ") size " << static_cast<uint16_t>(kCellWidth)
                  << "x" << static_cast<uint16_t>(kCellHeight) << "\n";
        auto button = std::make_unique<Button>(
            _eventDispatcher,
            FloatRectangle{buttonX, buttonY, kCellWidth, kCellHeight});
        button->registerEventHandler(
            [this, x, y, buttonX,
             buttonY](const MouseButtonEvent& mouseButtonEvent) -> void {
              auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch());
              if (mouseButtonEvent.down) {
                _eventProducer->produce(
                    std::make_unique<ClickEvent>(now, 0, x, y));
              }
            });
        buttons.emplace_back(std::move(button));
      }
    }
  };

  void render(const std::shared_ptr<TicTacToe>& ticTacToe) {
    _renderer->setRenderDrawColour(NamedColor::kWhite);
    _renderer->clear();

    _spriteRenderer->render(_board, 0, 0);

    for (uint8_t x :
         std::ranges::iota_view<uint8_t, uint8_t>{0, kCellColumns}) {
      for (uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellRows}) {
        const auto cellState = ticTacToe->getCellState(x, y);
        if (!cellState) {
          continue;
        }
        _spriteRenderer->render(
            cellState.value() == TicTacToe::Player::O ? _letterO : _letterX,
            static_cast<float>(x) * kCellWidth,
            static_cast<float>(y) * kCellHeight);
      }
    }
    _renderer->present();
  }

 private:
  Window _window{"Tic Tac Toe", kInitialWindowWidth, kInitialWindowHeight, 0};
  std::shared_ptr<EventProducer> _eventProducer;
  std::shared_ptr<EventDispatcher> _eventDispatcher;

  std::shared_ptr<Renderer> _renderer = std::make_shared<Renderer>(_window);
  std::shared_ptr<SpriteRenderer> _spriteRenderer =
      std::make_shared<SpriteRenderer>(_renderer);

  std::shared_ptr<Texture> _spriteSheet = std::make_shared<Texture>(
      *_renderer, &_binary_tic_tac_toe_png_start, ticTacToeSize());
  Sprite _board{_spriteSheet,
                {0, 0, kCellWidth* kCellColumns, kCellHeight* kCellRows}};
  Sprite _letterO{
      _spriteSheet,
      {kCellWidth * kCellColumns, kCellHeight, kCellWidth, kCellHeight}};
  Sprite _letterX{_spriteSheet,
                  {kCellWidth * kCellColumns, 0, kCellWidth, kCellHeight}};

  std::vector<std::unique_ptr<Button>> buttons;

  static const uint8_t kCellWidth = 128;
  static const uint8_t kCellHeight = kCellWidth;
  static const uint16_t kInitialWindowWidth = 384;
  static const uint16_t kInitialWindowHeight = kInitialWindowWidth;
};

auto main() -> int {
  try {
    SDL sdl;
    sdl.initSubSystem(SDL::kVideo);

    auto eventProducer = std::make_shared<EventProducer>();
    auto eventDispatcher = std::make_shared<EventDispatcher>(eventProducer);

    auto ticTacToe = std::make_shared<TicTacToe>();
    auto ticTacToeUI =
        std::make_shared<TicTacToeUI>(eventProducer, eventDispatcher);

    ticTacToeUI->render(ticTacToe);

    eventDispatcher->registerEventHandler<ClickEvent>(
        [=]([[maybe_unused]] const ClickEvent& clickEvent) -> void {
          ticTacToeUI->render(ticTacToe);
        });

    eventDispatcher->registerEventHandler<ClickEvent>(
        [=](const ClickEvent& clickEvent) -> void {
          ticTacToe->play(clickEvent.x, clickEvent.y);
        });

    eventDispatcher->run();
  } catch (std::exception& e) {
    std::cout << "Some kind of error happened!\n";
    std::cout << e.what() << "\n";
    return -1;
  }
  return 0;
}
